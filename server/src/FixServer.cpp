#include "FixServer.h"

#include <quickfix/fix44/ExecutionReport.h>
#include <quickfix/fix44/Logon.h>
#include <quickfix/fix44/Reject.h>

#include <iostream>

#include "common/ConfigManager.h"
#include "common/FixCoder.h"
#include "common/FixHelper.h"
#include "common/FixSessionVerity.h"
#include "common/Logger.h"
#include "common/MessageStore.h"

using namespace std;

FixSessionVerity sessionValidator;
FixCoder fixCoder;

FixHubServer::FixHubServer() {
  // 初始化 ConfigManager
  auto &config = ConfigManager::instance();
  config.load("config/server.json");
  config.loadAllowedCompIds("config/allowed_ids.txt");

  // 初始化 Logger
  auto &logger = Logger::instance();
  logger.init("fix_server.log", LogLevel::INFO);

  // 初始化 MessageStore
  auto &store = MessageStore::instance();
  store.init("storage/offline_msgs");

  // 初始化 Session 驗證規則
  unordered_map<string, string> allowedPairs;
  for (const auto &id : config.getAllowedCompIds()) {
    allowedPairs[id] = "HUB";   // 假設 server 為 HUB
  }
  sessionValidator.setAllowedPairs(allowedPairs);
}

void FixHubServer::onCreate(const FIX::SessionID &sessionID) {
  Logger::instance().info("🆕 Session created: " + sessionID.toString());
}

void FixHubServer::fromAdmin(const FIX::Message &message, const FIX::SessionID &sessionID) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) {
  FIX::MsgType msgType;
  message.getHeader().getField(msgType);

  if (msgType == FIX::MsgType_Logon) {
    sessionValidator.validateLogonOrThrow(message);
    Logger::instance().info("✅ Logon verified.");
  }
}

void FixHubServer::onLogon(const FIX::SessionID &sessionID) {
  auto &logger = Logger::instance();
  const string sender = sessionID.getSenderCompID().getString();
  logger.info("🔗 Logon: " + sender);

  // 取回離線訊息
  auto &store = MessageStore::instance();
  auto queued = store.getAndClearQueuedMessages(sender);
  for (const auto &rawMsg : queued) {
    FIX::Message fixMsg(rawMsg, FIX::DataDictionary("spec/FIX44.xml"), true);
    FIX::Session::sendToTarget(fixMsg, sessionID);
    logger.info("📦 Sent queued message to " + sender);
  }
}

void FixHubServer::onLogout(const FIX::SessionID &sessionID) {
  Logger::instance().info("❌ Logout: " + sessionID.toString());
}

void FixHubServer::fromApp(const FIX::Message &message, const FIX::SessionID &sessionID) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) {
  auto &config = ConfigManager::instance();
  auto &logger = Logger::instance();

  // 取出 MsgType
  string msgTypeStr;
  message.getHeader().getField(FIX::FIELD::MsgType, msgTypeStr);

  // 檢查是否支援該訊息類型
  if (!config.isMsgTypeSupported(msgTypeStr)) {
    FIX44::Reject reject;
    reject.set(FIX::Text("Unsupported MsgType=" + msgTypeStr));
    FIX::Session::sendToTarget(reject, sessionID);
    logger.warn("Rejected unsupported MsgType=" + msgTypeStr);
    return;
  }

  // 路由與轉換
  FixMessage fixMap;
  fixMap[35] = msgTypeStr;
  fixMap[49] = sessionID.getSenderCompID().getString();
  fixMap[56] = sessionID.getTargetCompID().getString();

  string error;
  if (!fixCoder.TransformMessage(fixMap, error)) {
    logger.error("Routing failed: " + error);
    return;
  }

  string target = fixCoder.GetTargetCompID(fixMap, error);
  if (target.empty()) {
    logger.error("No target found for message: " + error);
    return;
  }

  // 轉送
  FIX::Message forwardMsg(message);
  FixHelper::SetTagValue(fixMap, FIX::FIELD::TargetCompID, target);
  logger.info("🔁 Routed message to " + target);

  bool sent = false;
  for (const auto &sid : FIX::Session::getSessions()) {
    if (sid.getTargetCompID().getString() == target) {
      FIX::Session::sendToTarget(forwardMsg, sid);
      sent = true;
      break;
    }
  }

  if (!sent) {
    // 儲存離線訊息
    auto &store = MessageStore::instance();
    store.storeMessage(target, forwardMsg.toString());
    logger.warn("⚠️ Target offline. Message stored for " + target);
  }
}