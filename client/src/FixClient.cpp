#include "FixClient.h"
#include "json.hpp"

#include <QDebug>
#include <QThread>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;
using json = nlohmann::json;

// --- Helper function to build a FIX string ---
string buildFixString(const map<int, string> &fields) {
  string msg;
  for (const auto &pair : fields) {
    msg += to_string(pair.first) + "=" + pair.second + "\x01";
  }
  return msg;
}

// --- FixClient Implementation ---

// Constructor
FixClient::FixClient(QObject *parent) : QObject(parent), serverPort_(0) {
  connect(&socket_, &QTcpSocket::connected, this, &FixClient::onConnected);
  connect(&socket_, &QTcpSocket::readyRead, this, &FixClient::onReadyRead);
  connect(&socket_, &QTcpSocket::disconnected, this, &FixClient::onDisconnected);
}

// Load configuration from file
bool FixClient::loadConfig(const QString &filePath) {
  ifstream file(filePath.toStdString());
  if (!file.is_open()) {
    qCritical() << "Failed to open config file:" << filePath;
    return false;
  }

  try {
    json configData = json::parse(file);
    compId_ = QString::fromStdString(configData.value("CompID", ""));
    targetCompId_ = QString::fromStdString(configData.value("TargetCompID", ""));
    serverHost_ = QString::fromStdString(configData.value("ServerHost", "127.0.0.1"));
    serverPort_ = configData.value("ServerPort", 5555);

    if (compId_.isEmpty()) {
        qCritical() << "CompID is missing in config file.";
        return false;
    }

  } catch (const json::parse_error &e) {
    qCritical() << "Failed to parse config file:" << e.what();
    return false;
  }
  return true;
}


// Connect to server
void FixClient::connectToServer() {
  if (serverHost_.isEmpty() || serverPort_ == 0) {
      qCritical() << "Server configuration is not loaded or invalid.";
      return;
  }
  qInfo() << "Connecting to" << serverHost_ << ":" << serverPort_ << "with CompID:" << compId_;
  socket_.connectToHost(serverHost_, serverPort_);
}

// Send a test application message
void FixClient::sendTestMessage() {
    if (targetCompId_.isEmpty()) {
        qInfo() << "No TargetCompID specified, skipping test message.";
        return;
    }
    qInfo() << "Sending a test New Order message to" << targetCompId_;
    
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(gmtime(&in_time_t), "%Y%m%d-%H:%M:%S");
    string timestamp = ss.str();

    map<int, string> fields = {
        {8, "FIX.4.4"}, {35, "D"}, {49, compId_.toStdString()}, {56, "SERVER"},
        {128, targetCompId_.toStdString()}, {34, "2"}, {52, timestamp},
        {11, "TEST_ORDER_123"}, {55, "AAPL"}, {54, "1"}, {38, "100"}, {40, "2"}
    };
    sendMessage(buildFixString(fields));
}


// Send a raw FIX message string to the server
void FixClient::sendMessage(const std::string &rawMessage) {
  if (socket_.state() != QAbstractSocket::ConnectedState) {
    qWarning() << "Not connected to server.";
    return;
  }

  quint32 len = rawMessage.length();
  QByteArray packet;
  packet.append(reinterpret_cast<const char *>(&len), sizeof(len));
  packet.append(QByteArray::fromStdString(rawMessage));

  socket_.write(packet);
  qInfo() << "[Client] Sent:" << QString::fromStdString(rawMessage);
}

// Called when connection established
void FixClient::onConnected() {
  qInfo() << "Connected to server. Sending Logon.";

  auto now = chrono::system_clock::now();
  auto in_time_t = chrono::system_clock::to_time_t(now);
  stringstream ss;
  ss << put_time(gmtime(&in_time_t), "%Y%m%d-%H:%M:%S");
  string timestamp = ss.str();

  map<int, string> fields = {
      {8, "FIX.4.4"}, {35, "A"}, {49, compId_.toStdString()}, {56, "SERVER"},
      {34, "1"}, {52, timestamp}, {98, "0"}, {108, "30"}
  };
  sendMessage(buildFixString(fields));
}

// Called when data received
void FixClient::onReadyRead() {
  buffer_.append(socket_.readAll());

  while (buffer_.size() >= 4) {
    quint32 packetSize = 0;
    memcpy(&packetSize, buffer_.constData(), sizeof(quint32));

    if (buffer_.size() < static_cast<int>(4 + packetSize))
      break;

    QByteArray data = buffer_.mid(4, packetSize);
    buffer_.remove(0, 4 + packetSize);

    qInfo() << "[Client] Received:" << QString::fromUtf8(data);
  }
}

// Called when disconnected
void FixClient::onDisconnected() {
  qInfo() << "Disconnected from server.";
}