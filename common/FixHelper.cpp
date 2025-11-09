#include "FixHelper.h"

using namespace std;

// Get message type from tag 35
FixMessageType FixHelper::GetMsgType(const FixMessage &msg) {
  static const unordered_map<char, FixMessageType> typeMap = {
      {'D', FixMessageType::NewOrder},
      {'G', FixMessageType::OrderCancelReplace},
      {'F', FixMessageType::OrderCancel},
      {'8', FixMessageType::ExecutionReport},
      {'9', FixMessageType::CancelReject}};

  auto it = msg.find(TAG_MSG_TYPE);
  if (it == msg.end() || it->second.size() != 1)
    return FixMessageType::Unknown;

  char c = it->second[0];
  auto found = typeMap.find(c);
  return (found != typeMap.end()) ? found->second : FixMessageType::Unknown;
}

// Check if message type is routable
bool FixHelper::IsRoutableAppMessage(FixMessageType msgType) {
  static const unordered_set<FixMessageType> routable = {
      FixMessageType::NewOrder,
      FixMessageType::OrderCancelReplace,
      FixMessageType::OrderCancel,
      FixMessageType::ExecutionReport,
      FixMessageType::CancelReject};

  return routable.count(msgType) > 0;
}

// Get value of tag
string FixHelper::GetTagValue(const FixMessage &msg, int tag) {
  auto it = msg.find(tag);
  if (it != msg.end())
    return it->second;
  return "";
}

// Set value of tag
void FixHelper::SetTagValue(FixMessage &msg, int tag, const string &value) {
  msg[tag] = value;
}