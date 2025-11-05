#ifndef FIXHELPER_H
#define FIXHELPER_H

#include <map>
#include <string>

enum class FixMessageType {
  NewOrder = 'D',             // 35=D
  OrderCancelReplace = 'G',   // 35=G
  OrderCancel = 'F',          // 35=F
  ExecutionReport = '8',      // 35=8
  CancelReject = '9',         // 35=9
  Unknown = '\0'              // other or undefined type
};

using FixMessage = std::map<int, std::string>;

class FixHelper {
 public:
  // === Tag ===
  static const int TAG_MSG_TYPE = 35;
  static const int TAG_SENDER_COMP_ID = 49;
  static const int TAG_TARGET_COMP_ID = 56;
  static const int TAG_ON_BEHALF_OF_COMP_ID = 115;
  static const int TAG_DELIVER_TO_COMP_ID = 128;

  static FixMessageType GetMsgType(const FixMessage &msg);

  static bool IsRoutableAppMessage(FixMessageType msgType);

  static std::string GetTagValue(const FixMessage &msg, int tag);

  static void SetTagValue(FixMessage &msg, int tag, const std::string &value);
};

#endif   // FIXHELPER_H