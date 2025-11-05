#ifndef FIXSERVER_H
#define FIXSERVER_H

#include <QList>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class FixServer : public QTcpServer {
  Q_OBJECT

 public:
  explicit FixServer(QObject *parent = nullptr);   // Constructor
  bool start(quint16 port);                        // Start listening on port

 private slots:
  void onNewConnection();        // Handle new client connection
  void onClientDisconnected();   // Handle client disconnection
  void onReadyRead();            // Handle incoming data from clients

 private:
  QList<QTcpSocket *> m_clients;               // Connected clients
  QHash<QTcpSocket *, QByteArray> m_buffers;   // Pending data buffers
};

#endif   // FIXSERVER_H