#ifndef FIXCLIENT_H
#define FIXCLIENT_H

// Standard library
#include <memory>
#include <string>

// QuickFIX core headers
#include <quickfix/Application.h>
#include <quickfix/FileLog.h>
#include <quickfix/FileStore.h>
#include <quickfix/Session.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/SocketInitiator.h>

class FixClient : public FIX::Application {
 public:
  FixClient();
  ~FixClient() override = default;

  // Start the FIX client with a specified configuration file
  void start(const std::string &configPath);

  // QuickFIX application callbacks
  void onCreate(const FIX::SessionID &sessionID) override;
  void onLogon(const FIX::SessionID &sessionID) override;
  void onLogout(const FIX::SessionID &sessionID) override;
  void toAdmin(FIX::Message &message, const FIX::SessionID &sessionID) override;
  void fromAdmin(const FIX::Message &message, const FIX::SessionID &sessionID) override
      throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon);
  void toApp(FIX::Message &message, const FIX::SessionID &sessionID) override
      throw(FIX::DoNotSend);
  void fromApp(const FIX::Message &message, const FIX::SessionID &sessionID) override
      throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

  // Send a test order (NewOrderSingle)
  void sendTestOrder();

 private:
  FIX::SessionSettings settings_;                     // QuickFIX settings
  std::unique_ptr<FIX::SocketInitiator> initiator_;   // Socket initiator
  FIX::FileStoreFactory storeFactory_;                // Message store factory
  FIX::FileLogFactory logFactory_;                    // Log file factory
  FIX::SessionID activeSession_;                      // Current active session
  bool loggedOn_ = false;                             // Logon state
};

#endif   // FIXCLIENT_H