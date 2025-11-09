#include <QCoreApplication>

#include "FixServer.h"

// Entry point
int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);   // Initialize Qt core application

  FixServer server;          // Create server instance
  if (!server.start(5555))   // Start listening on port 5555
    return 1;                // Exit if server fails to start

  return app.exec();   // Run event loop
}