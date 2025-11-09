#ifndef FIXSERVER_H
#define FIXSERVER_H

#include <QHash>
#include <QList>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

// TCP server for FIX connections
class FixServer : public QTcpServer {
  Q_OBJECT

 public:
  // Constructor
  explicit FixServer(QObject *parent = nullptr);

  // Start listening on specified port
  bool start(quint16 port);

 private slots:
  // Handle new client connection
  void onNewConnection();

  // Handle client disconnection
  void onClientDisconnected();

  // Handle incoming data from client
  void onReadyRead();

 private:
  QList<QTcpSocket *> clients_;               // Connected client sockets
  QHash<QTcpSocket *, QByteArray> buffers_;   // Data buffers per client
};

#endif   // FIXSERVER_H