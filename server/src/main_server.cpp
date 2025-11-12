#include <quickfix/FileLog.h>
#include <quickfix/FileStore.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/SocketAcceptor.h>

#include <iostream>

#include "FixServer.h"
#include "common/ConfigManager.h"
#include "common/Logger.h"
#include "common/MessageStore.h"

using namespace std;

int main(int argc, char **argv) {
  try {
    // --- Load configuration paths ---
    string cfgFile = (argc > 1) ? argv[1] : "config/server.cfg";
    string jsonConfig = "config/server.json";
    string allowedIdsFile = "config/allowed_ids.txt";
    string msgStoragePath = "storage/offline_msgs";
    string logFile = "logs/fix_server.log";

    // --- Initialize logger ---
    auto &logger = Logger::instance();
    logger.init(logFile, LogLevel::INFO);
    logger.info("===== FIX Hub Server Starting =====");

    // --- Initialize configuration manager ---
    auto &config = ConfigManager::instance();
    config.load(jsonConfig);
    config.loadAllowedCompIds(allowedIdsFile);

    // --- Initialize message store ---
    auto &store = MessageStore::instance();
    store.init(msgStoragePath);

    // --- Setup FIX components ---
    FIX::SessionSettings settings(cfgFile);
    FIX::FileStoreFactory storeFactory(settings);
    FIX::FileLogFactory logFactory(settings);

    // --- Start FIX server ---
    FixHubServer app;
    FIX::SocketAcceptor acceptor(app, storeFactory, settings, logFactory);

    logger.info("Listening for FIX connections...");
    acceptor.start();

    // --- Command loop ---
    cout << "Type 'quit' to stop server, 'list' to show sessions." << endl;
    string cmd;
    while (getline(cin, cmd)) {
      if (cmd == "quit" || cmd == "exit") break;
      if (cmd == "list") app.printSessions();
    }

    // --- Stop server ---
    acceptor.stop();
    logger.info("===== FIX Hub Server Stopped =====");

  } catch (const exception &e) {
    cerr << "Exception: " << e.what() << endl;
    Logger::instance().error(string("Startup exception: ") + e.what());
    return 1;
  }

  return 0;
}