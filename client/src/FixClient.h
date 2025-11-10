#ifndef FIXCLIENT_H
#define FIXCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <string>

// TCP client for FIX connections
class FixClient : public QObject {
  Q_OBJECT

 public:
  // Constructor
  explicit FixClient(QObject *parent = nullptr);

  // Load configuration from file
  bool loadConfig(const QString &filePath);

  // Connect to FIX server using loaded config
  void connectToServer();

  // Send a test application message
  void sendTestMessage();

 private slots:
  // Called when connection established
  void onConnected();

  // Called when data is received
  void onReadyRead();

  // Called when disconnected
  void onDisconnected();

 private:
  // Send a raw FIX message string to the server
  void sendMessage(const std::string &rawMessage);

  QTcpSocket socket_;          // TCP socket instance
  QByteArray buffer_;          // Data buffer for incoming messages

  // Configuration
  QString compId_;
  QString targetCompId_;
  QString serverHost_;
  quint16 serverPort_;
};

#endif   // FIXCLIENT_H