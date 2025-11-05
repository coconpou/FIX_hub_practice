#ifndef FIXCLIENT_H
#define FIXCLIENT_H

#include <QObject>
#include <QTcpSocket>

class FixClient : public QObject {
  Q_OBJECT

 public:
  explicit FixClient(QObject *parent = nullptr);             // Constructor
  void connectToServer(const QString &host, quint16 port);   // Connect to server
  void sendMessage(const QByteArray &data);                  // Send message to server

 private slots:
  void onConnected();      // Called when connected
  void onReadyRead();      // Called when data received
  void onDisconnected();   // Called when disconnected

 private:
  QTcpSocket m_socket;   // TCP socket instance
};

#endif   // FIXCLIENT_H