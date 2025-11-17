#include "FixHelper.h"

#include <sstream>
using namespace std;

//  static tag constant definitions
const int FixHelper::TAG_MSG_TYPE = 35;
const int FixHelper::TAG_SENDER_COMP_ID = 49;
const int FixHelper::TAG_TARGET_COMP_ID = 56;
const int FixHelper::TAG_ON_BEHALF_OF_COMP_ID = 115;
const int FixHelper::TAG_DELIVER_TO_COMP_ID = 128;

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

// Check if the FIX message is routable

bool FixHelper::IsRoutableAppMessage(FixMessageType msgType) {
  static const unordered_set<FixMessageType> routable = {
      FixMessageType::NewOrder,
      FixMessageType::OrderCancelReplace,
      FixMessageType::OrderCancel,
      FixMessageType::ExecutionReport,
      FixMessageType::CancelReject};

  return routable.count(msgType) > 0;
}

// Get value by FIX tag

string FixHelper::GetTagValue(const FixMessage &msg, int tag) {
  auto it = msg.find(tag);
  if (it != msg.end())
    return it->second;
  return "";
}

// Set FIX tag value

void FixHelper::SetTagValue(FixMessage &msg, int tag, const string &value) {
  msg[tag] = value;
}

// Parse Raw FIX → map<int,string>

FixMessage FixHelper::ParseRawFix(const std::string &raw, char delimiter) {
  FixMessage msg;

  string normalized = raw;
  for (char &c : normalized) {
    if (c == '\x01') c = delimiter;
  }

  stringstream ss(normalized);
  string token;

  while (getline(ss, token, delimiter)) {
    if (token.empty()) continue;

    auto pos = token.find('=');
    if (pos == string::npos) continue;

    string tagStr = token.substr(0, pos);
    string value = token.substr(pos + 1);

    try {
      int tag = stoi(tagStr);
      msg[tag] = value;
    } catch (...) {
      continue;
    }
  }

  return msg;
}

// Convert map<int,string> → raw FIX

string FixHelper::ToRawFix(const FixMessage &msg, char delimiter) {
  ostringstream oss;

  for (const auto &p : msg) {
    oss << p.first << '=' << p.second << delimiter;
  }

  return oss.str();
}