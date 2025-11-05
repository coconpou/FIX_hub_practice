#include "FixClient.h"

#include <QDebug>
#include <QThread>

FixClient::FixClient(QObject *parent) : QObject(parent) {
  connect(&m_socket, &QTcpSocket::connected, this, &FixClient::onConnected);         // Handle connection event
  connect(&m_socket, &QTcpSocket::readyRead, this, &FixClient::onReadyRead);         // Handle incoming data
  connect(&m_socket, &QTcpSocket::disconnected, this, &FixClient::onDisconnected);   // Handle disconnection
}

void FixClient::connectToServer(const QString &host, quint16 port) {
  qInfo() << "Connecting to" << host << ":" << port;   // Log connection attempt
  m_socket.connectToHost(host, port);                  // Connect to server
}

void FixClient::sendMessage(const QByteArray &data) {
  if (m_socket.state() != QAbstractSocket::ConnectedState) {
    qWarning() << "Not connected to server.";   // Warn if not connected
    return;
  }

  const int PACKET_SIZE = 16;   // Packet chunk size
  int offset = 0;

  while (offset < data.size()) {
    QByteArray part = data.mid(offset, PACKET_SIZE);   // Extract data chunk
    offset += PACKET_SIZE;

    quint32 len = part.size();                                        // Packet length
    QByteArray header(reinterpret_cast<char *>(&len), sizeof(len));   // Header for packet
    m_socket.write(header);                                           // Send header
    m_socket.write(part);                                             // Send body
    m_socket.flush();                                                 // Ensure immediate send

    qInfo() << "[Client] Sent packet:" << part;   // Log sent packet
    QThread::msleep(50);                          // Simulate delay between packets
  }
}

void FixClient::onConnected() {
  qInfo() << "Connected to server.";                                         // Log connection success
  QByteArray message = "This is a long message sent in multiple packets.";   // Sample message
  sendMessage(message);                                                      // Send demo message after connecting
}

void FixClient::onReadyRead() {
  qInfo() << "[Client] Server says:" << m_socket.readAll();   // Print server response
}

void FixClient::onDisconnected() {
  qInfo() << "Disconnected from server.";   // Log disconnection
}