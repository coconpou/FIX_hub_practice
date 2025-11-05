#include "FixCoder.h"
#include "FixHelper.h" 

FixCoder::FixCoder() {}

void FixCoder::AddRoute(const std::string& compId, const std::string& customId)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _compIdToCustomId[compId] = customId;
    _customIdToCompId[customId] = compId;
}

// from tag 128 to get CompID
std::string FixCoder::GetTargetCompID(const FixMessage& msg, std::string& error) const
{
    // get Tag 128/deliverTo id
    std::string deliverTo = FixHelper::GetTagValue(msg, FixHelper::TAG_DELIVER_TO_COMP_ID);
    
    if (deliverTo.empty()) {
        error = "Tag 128 (DeliverToCompID) is missing.";
        return "";
    }

    // find the opposed CompID
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _customIdToCompId.find(deliverTo);
    
    if (it == _customIdToCompId.end()) {
        error = "Tag 128 (" + deliverTo + ") not found in routing table.";
        return "";
    }
    
    return it->second; // "BROKER"
}


bool FixCoder::TransformMessage(FixMessage& msg, std::string& error)
{
    // get labels
    std::string senderCompId = FixHelper::GetTagValue(msg, FixHelper::TAG_SENDER_COMP_ID); // Tag 49
    std::string deliverToId = FixHelper::GetTagValue(msg, FixHelper::TAG_DELIVER_TO_COMP_ID); // Tag 128

    if (senderCompId.empty()) {
        error = "Tag 49 (SenderCompID) is missing, cannot transform.";
        return false;
    }
    if (deliverToId.empty()) {
        error = "Tag 128 (DeliverToCompID) is missing, cannot transform.";
        return false;
    }

    std::string newTargetCompId;
    std::string newOnBehalfOfId;

    {
        std::lock_guard<std::mutex> lock(_mutex);

        // find New Target: Tag 56
        // 128=BR1 -> find "BR1" -> get "BROKER"
        auto it_target = _customIdToCompId.find(deliverToId);
        if (it_target == _customIdToCompId.end()) {
            error = "Routing failed: CustomID (Tag 128) '" + deliverToId + "' not found.";
            return false;
        }
        newTargetCompId = it_target->second; // e.g., "BROKER"

        // Search new OnBehalfOfCompID: tag 115
        // e.g. 49=ABC, search "ABC" and get "99900A"
        auto it_onBehalfOf = _compIdToCustomId.find(senderCompId);
        if (it_onBehalfOf == _compIdToCustomId.end()) {
            error = "Routing failed: CompID (Tag 49) '" + senderCompId + "' not found.";
            return false;
        }
        newOnBehalfOfId = it_onBehalfOf->second; // e.g., "99900A"
    }

    FixHelper::SetTagValue(msg, FixHelper::TAG_SENDER_COMP_ID, "HUB");                 // 49=HUB
    FixHelper::SetTagValue(msg, FixHelper::TAG_TARGET_COMP_ID, newTargetCompId);       // 56=BROKER
    FixHelper::SetTagValue(msg, FixHelper::TAG_ON_BEHALF_OF_COMP_ID, newOnBehalfOfId); // 115=99900A

    // get rid of Tag 128
    msg.erase(FixHelper::TAG_DELIVER_TO_COMP_ID);

    return true;
}