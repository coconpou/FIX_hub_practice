#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

#include "FixClient.h"

// Entry point
int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  FixClient client;
  if (!client.loadConfig("config/fix_client.cfg")) {
    qCritical() << "Failed to load configuration. Exiting.";
    return 1;
  }

  client.connectToServer();

  // Use a timer to send a test message after 2 seconds
  QTimer::singleShot(2000, &client, &FixClient::sendTestMessage);

  return app.exec();
}