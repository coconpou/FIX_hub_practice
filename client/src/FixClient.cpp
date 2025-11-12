#include "FixClient.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#include "common/Logger.h"

using namespace std;

// Constructor
FixClient::FixClient()
    : storeFactory_(FIX::SessionSettings()), logFactory_(FIX::SessionSettings()) {}

// Start the FIX client with configuration file
void FixClient::start(const string &configPath) {
  try {
    // --- Initialize logger ---
    Logger::instance().init("logs/fix_client.log", LogLevel::INFO);
    Logger::instance().info("===== FIX Client Starting =====");

    // --- Load QuickFIX settings ---
    settings_ = FIX::SessionSettings(configPath);
    FIX::FileStoreFactory storeFactory(settings_);
    FIX::FileLogFactory logFactory(settings_);

    initiator_ = make_unique<FIX::SocketInitiator>(*this, storeFactory, settings_, logFactory);
    initiator_->start();
    Logger::instance().info("Client initiator started, waiting for Logon...");

    // --- Wait until logon is successful ---
    while (!loggedOn_) {
      this_thread::sleep_for(chrono::milliseconds(500));
    }

    Logger::instance().info("Logon successful. Type 'send' to send test order or 'quit' to exit.");

    // --- Command loop ---
    string cmd;
    while (getline(cin, cmd)) {
      if (cmd == "quit" || cmd == "exit") break;
      if (cmd == "send") sendTestOrder();
    }

    // --- Stop initiator ---
    initiator_->stop();
    Logger::instance().info("===== FIX Client Stopped =====");
  } catch (const exception &e) {
    Logger::instance().error(string("Exception in FixClient::start: ") + e.what());
  }
}

// --- QuickFIX callbacks ---

void FixClient::onCreate(const FIX::SessionID &sessionID) {
  Logger::instance().info("[Client] Session created: " + sessionID.toString());
}

void FixClient::onLogon(const FIX::SessionID &sessionID) {
  Logger::instance().info("[Client] Logon successful: " + sessionID.toString());
  activeSession_ = sessionID;
  loggedOn_ = true;
}

void FixClient::onLogout(const FIX::SessionID &sessionID) {
  Logger::instance().warn("[Client] Logout: " + sessionID.toString());
  loggedOn_ = false;
}

void FixClient::toAdmin(FIX::Message &message, const FIX::SessionID &sessionID) {
  Logger::instance().debug("[Client->Admin] " + message.toString());
}

void FixClient::fromAdmin(const FIX::Message &message, const FIX::SessionID &sessionID) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) {
  Logger::instance().debug("[Admin->Client] " + message.toString());
}

void FixClient::toApp(FIX::Message &message, const FIX::SessionID &sessionID) throw(FIX::DoNotSend) {
  Logger::instance().debug("[Client->App] " + message.toString());
}

void FixClient::fromApp(const FIX::Message &message, const FIX::SessionID &sessionID) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) {
  Logger::instance().info("[App->Client] " + message.toString());
}

// --- Utility functions ---

void FixClient::sendTestOrder() {
  if (!loggedOn_) {
    Logger::instance().warn("Cannot send order: not logged on.");
    return;
  }

  // Create FIX44::NewOrderSingle
  FIX44::NewOrderSingle order(
      FIX::ClOrdID("TEST_ORDER_001"),
      FIX::HandlInst('1'),
      FIX::Symbol("AAPL"),
      FIX::Side(FIX::Side_BUY),
      FIX::TransactTime(),
      FIX::OrdType(FIX::OrdType_MARKET));

  order.set(FIX::OrderQty(100));
  order.set(FIX::TimeInForce(FIX::TimeInForce_DAY));

  try {
    FIX::Session::sendToTarget(order, activeSession_);
    Logger::instance().info("[Client] Sent test order (NewOrderSingle) to server.");
  } catch (const exception &e) {
    Logger::instance().error(string("[Client] Failed to send order: ") + e.what());
  }
}