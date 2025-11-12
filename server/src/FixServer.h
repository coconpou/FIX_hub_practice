#ifndef FIXSERVER_H
#define FIXSERVER_H

// Standard library
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// QuickFIX core headers
#include <quickfix/Application.h>
#include <quickfix/FileLog.h>
#include <quickfix/FileStore.h>
#include <quickfix/Session.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/SocketAcceptor.h>

// Core modules
#include "common/ConfigManager.h"
#include "common/FixCoder.h"
#include "common/FixHelper.h"
#include "common/FixSessionVerity.h"
#include "common/Logger.h"
#include "common/MessageStore.h"

// FIX message hub server implementing FIX::Application
class FixHubServer : public FIX::Application {
 public:
  // Constructor / Destructor
  FixHubServer();
  ~FixHubServer() override = default;

  // QuickFIX session callbacks
  void onCreate(const FIX::SessionID &sessionID) override;
  void onLogon(const FIX::SessionID &sessionID) override;
  void onLogout(const FIX::SessionID &sessionID) override;

  // Admin-level message handling
  void toAdmin(FIX::Message &msg, const FIX::SessionID &sessionID) override;
  void fromAdmin(const FIX::Message &msg, const FIX::SessionID &sessionID) override
      throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon);

  // Application-level message handling
  void toApp(FIX::Message &msg, const FIX::SessionID &sessionID) override
      throw(FIX::DoNotSend);
  void fromApp(const FIX::Message &msg, const FIX::SessionID &sessionID) override
      throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

  // Print all active sessions
  void printSessions();

 private:
  // --- Module references ---
  ConfigManager &config_ = ConfigManager::instance();   // Configuration manager
  FixSessionVerity sessionValidator_;                   // Session header validator
  FixCoder coder_;                                      // Message routing encoder/decoder
};

#endif   // FIXSERVER_H