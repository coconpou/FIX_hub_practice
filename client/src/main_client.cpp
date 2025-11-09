#include <QCoreApplication>

#include "FixClient.h"

// Entry point
int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);   // Initialize Qt core application

  FixClient client;                            // Create FIX client instance
  client.connectToServer("127.0.0.1", 5555);   // Connect to FIX server

  return app.exec();   // Start event loop
}