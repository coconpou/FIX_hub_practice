#include "FixCoder.h"

#include <iostream>

#include "FixHelper.h"

using namespace std;

FixCoder::FixCoder() {}
FixCoder::~FixCoder() = default;

void FixCoder::LoadFromConfig(const ConfigManager &cfg) {
  lock_guard<mutex> lock(_mutex);

  deliverToTarget_.clear();
  senderToBehalf_.clear();

  // Load routing_rules
  for (const auto &r : cfg.routingRules()) {
    deliverToTarget_[r.deliverId] = r.targetCompId;
    cout << "[INFO] Routing: 128=" << r.deliverId
         << " -> 56=" << r.targetCompId << endl;
  }

  // Load on_behalf_rules
  for (const auto &r : cfg.behalfRules()) {
    senderToBehalf_[r.senderCompId] = r.onBehalfId;
    cout << "[INFO] OnBehalf: 49=" << r.senderCompId
         << " -> 115=" << r.onBehalfId << endl;
  }
}

bool FixCoder::TransformMessage(FixMessage &msg, string &error) {
  lock_guard<mutex> lock(_mutex);

  string sender = FixHelper::GetTagValue(msg, FixHelper::TAG_SENDER_COMP_ID);        // 49
  string deliver = FixHelper::GetTagValue(msg, FixHelper::TAG_DELIVER_TO_COMP_ID);   // 128

  if (sender.empty()) {
    error = "Missing Tag49 (SenderCompID)";
    return false;
  }
  if (deliver.empty()) {
    error = "Missing Tag128 (DeliverToCompID)";
    return false;
  }

  // Tag128 → Tag56
  auto itRoute = deliverToTarget_.find(deliver);
  if (itRoute == deliverToTarget_.end()) {
    error = "Routing failed: deliver_id '" + deliver + "' not found in config.json";
    return false;
  }
  string newTarget = itRoute->second;

  // Tag49 → Tag115
  auto itBehalf = senderToBehalf_.find(sender);
  if (itBehalf == senderToBehalf_.end()) {
    error = "OnBehalfOf lookup failed: sender '" + sender + "' not found in config.json";
    return false;
  }
  string newBehalf = itBehalf->second;

  // Apply fix routing
  FixHelper::SetTagValue(msg, FixHelper::TAG_SENDER_COMP_ID, "HUB");
  FixHelper::SetTagValue(msg, FixHelper::TAG_TARGET_COMP_ID, newTarget);
  FixHelper::SetTagValue(msg, FixHelper::TAG_ON_BEHALF_OF_COMP_ID, newBehalf);

  msg.erase(FixHelper::TAG_DELIVER_TO_COMP_ID);

  return true;
}