#include <unistd.h>

#include <iostream>

#include "ConfigManager.h"
#include "FixCoder.h"
#include "FixServer.h"
#include "FixSessionVerity.h"
#include "Logger.h"
#include "MessageStore.h"

int main() {
  std::cout << "[INFO] FixHubServer starting..." << std::endl;
  Logger::instance().init("hub.log", LogLevel::INFO);

  ConfigManager &cfg = ConfigManager::instance();
  try {
    cfg.load("config/config.json");
    std::cout << "[INFO] Config loaded." << std::endl;
  } catch (const std::exception &ex) {
    std::cerr << "[ERROR] Failed to load config: " << ex.what() << std::endl;
    return 1;
  }

  MessageStore::instance().init("./queue");

  FixCoder coder;
  coder.LoadFromConfig(cfg);
  std::cout << "[INFO] Routing loaded from config." << std::endl;

  FixSessionVerity verity;

  verity.addAllowedPair("CLIENT1", "BROKER");
  verity.addAllowedPair("CLIENT2", "BROKER2");

  int port = 5001;
  FixServer server(&coder, &verity, port);

  std::cout << "[INFO] Server is running on port " << port << std::endl;
  server.start();

  while (true) {
    sleep(1);
  }
}