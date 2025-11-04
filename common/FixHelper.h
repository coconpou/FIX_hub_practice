#ifndef FIXHELPER_H
#define FIXHELPER_H

#include <string>
#include <map>

enum class FixMessageType {
    NewOrder = 'D',          // 35=D 委託
    OrderCancelReplace = 'G',// 35=G 改單
    OrderCancel = 'F',       // 35=F 刪單
    ExecutionReport = '8',   // 35=8 回報(委託/成交)
    CancelReject = '9',      // 35=9 刪改失敗
    Unknown = '\0'           // undefined type
};

using FixMessage = std::map<int, std::string>;

class FixHelper 
{
public:
    // === Tag ===
    static const int TAG_MSG_TYPE = 35;
    static const int TAG_SENDER_COMP_ID = 49;
    static const int TAG_TARGET_COMP_ID = 56;
    static const int TAG_ON_BEHALF_OF_COMP_ID = 115;
    static const int TAG_DELIVER_TO_COMP_ID = 128;

    static FixMessageType GetMsgType(const FixMessage& msg);

    static bool IsRoutableAppMessage(FixMessageType msgType);

    static std::string GetTagValue(const FixMessage& msg, int tag);

    static void SetTagValue(FixMessage& msg, int tag, const std::string& value);

};

#endif