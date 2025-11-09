#include "FixClient.h"

#include <QDebug>
#include <QThread>

using namespace std;

// Constructor
FixClient::FixClient(QObject *parent) : QObject(parent) {
  connect(&socket_, &QTcpSocket::connected, this, &FixClient::onConnected);
  connect(&socket_, &QTcpSocket::readyRead, this, &FixClient::onReadyRead);
  connect(&socket_, &QTcpSocket::disconnected, this, &FixClient::onDisconnected);
}

// Connect to server
void FixClient::connectToServer(const QString &host, quint16 port) {
  qInfo() << "Connecting to" << host << ":" << port;
  socket_.connectToHost(host, port);
}

// Send message to server
void FixClient::sendMessage(const QByteArray &data) {
  if (socket_.state() != QAbstractSocket::ConnectedState) {
    qWarning() << "Not connected to server.";
    return;
  }

  const int PACKET_SIZE = 16;   // Packet chunk size
  int offset = 0;

  while (offset < data.size()) {
    QByteArray part = data.mid(offset, PACKET_SIZE);
    offset += PACKET_SIZE;

    quint32 len = part.size();
    QByteArray header(reinterpret_cast<char *>(&len), sizeof(len));

    socket_.write(header);
    socket_.write(part);
    socket_.flush();

    qInfo() << "[Client] Sent packet:" << part;
    QThread::msleep(50);   // Simulate delay between packets
  }
}

// Called when connection established
void FixClient::onConnected() {
  qInfo() << "Connected to server.";
  QByteArray message = "This is a long message sent in multiple packets.";
  sendMessage(message);
}

// Called when data received
void FixClient::onReadyRead() {
  qInfo() << "[Client] Server says:" << socket_.readAll();
}

// Called when disconnected
void FixClient::onDisconnected() {
  qInfo() << "Disconnected from server.";
}