#include "FixHelper.h"

FixMessageType FixHelper::GetMsgType(const FixMessage& msg) 
{

    std::string msgTypeStr = FixHelper::GetTagValue(msg, TAG_MSG_TYPE);
    
    if (msgTypeStr.empty() || msgTypeStr.length() > 1) {
        return FixMessageType::Unknown;
    }

    // string[0] == char
    char msgTypeChar = msgTypeStr[0];

    switch (msgTypeChar) 
    {
        case 'D': return FixMessageType::NewOrder;
        case 'G': return FixMessageType::OrderCancelReplace;
        case 'F': return FixMessageType::OrderCancel;
        case '8': return FixMessageType::ExecutionReport;
        case '9': return FixMessageType::CancelReject;

        default:
            return FixMessageType::Unknown;
    }
}


bool FixHelper::IsRoutableAppMessage(FixMessageType msgType)
{
    // check enum value
    switch (msgType)
    {
        case FixMessageType::NewOrder:
        case FixMessageType::OrderCancelReplace:
        case FixMessageType::OrderCancel:
        case FixMessageType::ExecutionReport:
        case FixMessageType::CancelReject:
            return true;
        
        default:

            return false;
    }
}


std::string FixHelper::GetTagValue(const FixMessage& msg, int tag)
{

    auto it = msg.find(tag);

    if (it != msg.end()) {
        return it->second;
    }

    return "";
}


void FixHelper::SetTagValue(FixMessage& msg, int tag, const std::string& value)
{

    msg[tag] = value;
}