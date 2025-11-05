#include "FixServer.h"

#include <QDebug>

FixServer::FixServer(QObject *parent) : QTcpServer(parent) {
  connect(this, &QTcpServer::newConnection, this, &FixServer::onNewConnection);   // Handle new client connection
}

bool FixServer::start(quint16 port) {
  if (!listen(QHostAddress::Any, port)) {
    qCritical() << "Server failed to start:" << errorString();   // Log start failure
    return false;
  }
  qInfo() << "Server started on port" << port;   // Log successful start
  return true;
}

void FixServer::onNewConnection() {
  QTcpSocket *client = nextPendingConnection();
  if (!client) return;

  m_clients.append(client);
  m_buffers[client].clear();

  qInfo() << "New client connected from" << client->peerAddress().toString();   // Log new connection

  connect(client, &QTcpSocket::readyRead, this, &FixServer::onReadyRead);               // Handle incoming data
  connect(client, &QTcpSocket::disconnected, this, &FixServer::onClientDisconnected);   // Handle client disconnect
}

void FixServer::onClientDisconnected() {
  QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
  if (!client) return;

  qInfo() << "Client disconnected:" << client->peerAddress().toString();   // Log disconnection
  m_clients.removeAll(client);
  m_buffers.remove(client);
  client->deleteLater();   // Clean up socket
}

void FixServer::onReadyRead() {
  QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
  if (!client) return;

  QByteArray &buffer = m_buffers[client];
  buffer.append(client->readAll());   // Append new data to buffer

  // Packet format: [4-byte length header] + [payload]
  while (buffer.size() >= 4) {
    quint32 packetSize = 0;
    memcpy(&packetSize, buffer.constData(), sizeof(quint32));

    if (buffer.size() < static_cast<int>(4 + packetSize))
      break;   // Wait for full packet

    QByteArray data = buffer.mid(4, packetSize);
    buffer.remove(0, 4 + packetSize);

    qInfo() << "[Server] Received packet:" << QString::fromUtf8(data);   // Log received message
    client->write("ACK\n");                                              // Send acknowledgment
  }
}