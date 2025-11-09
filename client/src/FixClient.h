#ifndef FIXCLIENT_H
#define FIXCLIENT_H

#include <QObject>
#include <QTcpSocket>

// TCP client for FIX connections
class FixClient : public QObject {
  Q_OBJECT

 public:
  // Constructor
  explicit FixClient(QObject *parent = nullptr);

  // Connect to FIX server
  void connectToServer(const QString &host, quint16 port);

  // Send FIX message to server
  void sendMessage(const QByteArray &data);

 private slots:
  // Called when connection established
  void onConnected();

  // Called when data is received
  void onReadyRead();

  // Called when disconnected
  void onDisconnected();

 private:
  QTcpSocket socket_;   // TCP socket instance
};

#endif   // FIXCLIENT_H