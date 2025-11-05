#include <QCoreApplication>

#include "FixClient.h"

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);   // Initialize Qt core application

  FixClient client;                            // Create client instance
  client.connectToServer("127.0.0.1", 5555);   // Connect to server

  return app.exec();   // Start event loop
}