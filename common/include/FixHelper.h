#ifndef FIXHELPER_H
#define FIXHELPER_H

#include <quickfix/Message.h>

#include <algorithm>
#include <map>
#include <string>
#include <unordered_set>

// FIX message types
enum class FixMessageType {
  NewOrder = 'D',             // 35=D
  OrderCancelReplace = 'G',   // 35=G
  OrderCancel = 'F',          // 35=F
  ExecutionReport = '8',      // 35=8
  CancelReject = '9',         // 35=9
  Unknown = '\0'              // undefined
};

// FIX message map (Tag -> Value)
using FixMessage = std::map<int, std::string>;

class FixHelper {
 public:
  // FIX tag constants
  static const int TAG_MSG_TYPE;
  static const int TAG_SENDER_COMP_ID;
  static const int TAG_TARGET_COMP_ID;
  static const int TAG_ON_BEHALF_OF_COMP_ID;
  static const int TAG_DELIVER_TO_COMP_ID;

  // Get message type from tag 35
  static FixMessageType GetMsgType(const FixMessage &msg);

  // Check if message is routable
  static bool IsRoutableAppMessage(FixMessageType msgType);

  // Get value by tag
  static std::string GetTagValue(const FixMessage &msg, int tag);

  // Set value by tag
  static void SetTagValue(FixMessage &msg, int tag, const std::string &value);
  static FixMessage ParseRawFix(const std::string &raw,
                                char delimiter = '|');

  static std::string ToRawFix(const FixMessage &msg,
                              char delimiter = '|');

  static FIX::Message ToQuickFix(const FixMessage &msg);
};

#endif   // FIXHELPER_H