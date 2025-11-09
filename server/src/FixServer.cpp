#include "FixServer.h"

#include <QDebug>
#include <QHostAddress>

using namespace std;

// Constructor
FixServer::FixServer(QObject *parent) : QTcpServer(parent) {
  connect(this, &QTcpServer::newConnection, this, &FixServer::onNewConnection);
}

// Start listening on specified port
bool FixServer::start(quint16 port) {
  if (!listen(QHostAddress::Any, port)) {
    qCritical() << "Server failed to start:" << errorString();
    return false;
  }
  qInfo() << "Server started on port" << port;
  return true;
}

// Handle new client connection
void FixServer::onNewConnection() {
  QTcpSocket *client = nextPendingConnection();
  if (!client) return;

  clients_.append(client);
  buffers_[client].clear();

  qInfo() << "New client connected from" << client->peerAddress().toString();

  connect(client, &QTcpSocket::readyRead, this, &FixServer::onReadyRead);
  connect(client, &QTcpSocket::disconnected, this, &FixServer::onClientDisconnected);
}

// Handle client disconnection
void FixServer::onClientDisconnected() {
  QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
  if (!client) return;

  qInfo() << "Client disconnected:" << client->peerAddress().toString();
  clients_.removeAll(client);
  buffers_.remove(client);
  client->deleteLater();
}

// Handle incoming data from client
void FixServer::onReadyRead() {
  QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
  if (!client) return;

  QByteArray &buffer = buffers_[client];
  buffer.append(client->readAll());

  // Packet format: [4-byte length header] + [payload]
  while (buffer.size() >= 4) {
    quint32 packetSize = 0;
    memcpy(&packetSize, buffer.constData(), sizeof(quint32));

    if (buffer.size() < static_cast<int>(4 + packetSize))
      break;   // Wait for full packet

    QByteArray data = buffer.mid(4, packetSize);
    buffer.remove(0, 4 + packetSize);

    qInfo() << "[Server] Received packet:" << QString::fromUtf8(data);
    client->write("ACK\n");
  }
}