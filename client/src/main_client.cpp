#include <iostream>
#include <string>

#include "FixClient.h"
#include "common/Logger.h"

int main(int argc, char **argv) {
  try {
    // Default configuration path
    std::string configPath = "config/client.cfg";
    if (argc > 1) {
      configPath = argv[1];
    }

    // Initialize global logger for the client
    auto &logger = Logger::instance();
    logger.init("logs/fix_client_main.log", LogLevel::INFO);
    logger.info("========== FIX CLIENT START ==========");
    logger.info("Using configuration file: " + configPath);

    // Start the FIX client
    FixClient client;
    client.start(configPath);

    logger.info("========== FIX CLIENT EXIT ==========");
  } catch (const std::exception &e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    Logger::instance().error(std::string("Fatal error: ") + e.what());
    return 1;
  }

  return 0;
}