#include "FixServer.h"

#include <QDebug>
#include <QHostAddress>
#include <QSocketNotifier>
#include <csignal>
#include <unistd.h>

using namespace std;

// Define static pipe member
int FixServer::sig_pipe[2];

// Constructor
FixServer::FixServer(QObject *parent) : QTcpServer(parent) {
  connect(this, &QTcpServer::newConnection, this, &FixServer::onNewConnection);
  setupSignalHandlers();
}

// Setup signal handling using the self-pipe trick
void FixServer::setupSignalHandlers() {
  if (::pipe(sig_pipe)) {
    qFatal("Failed to create pipe for signal handling.");
  }

  auto *notifier = new QSocketNotifier(sig_pipe[0], QSocketNotifier::Read, this);
  connect(notifier, &QSocketNotifier::activated, this, &FixServer::handleStatusSignal);

  struct sigaction sa;
  sa.sa_handler = FixServer::statusSignalHandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_flags |= SA_RESTART; // Restart syscalls if possible

  if (sigaction(SIGUSR1, &sa, 0) != 0) {
    qFatal("Failed to register signal handler for SIGUSR1.");
  }
}

// Static signal handler - writes to the pipe
void FixServer::statusSignalHandler(int) {
  char a = 1;
  // Use write and ignore result to be safe in a signal handler
  (void)::write(sig_pipe[1], &a, sizeof(a));
}

// Slot to handle the status dump
void FixServer::handleStatusSignal() {
  // Read from the pipe to reset the notifier
  char tmp;
  (void)::read(sig_pipe[0], &tmp, sizeof(tmp));

  Logger::instance().info("--- Session Status Dump Requested ---");
  Logger::instance().info("Total connected clients: " + to_string(clients_.size()));

  for (QTcpSocket *client : clients_) {
    string compId = socketToCompIdMap_.contains(client) ? socketToCompIdMap_[client] : "N/A (Authenticating)";
    string ip = client->peerAddress().toString().toStdString();
    string port = to_string(client->peerPort());
    string connectedAt = connectionTimeMap_.contains(client) ? connectionTimeMap_[client].toString("yyyy-MM-dd hh:mm:ss").toStdString() : "N/A";

    string status = "  -> CompID: " + compId + ", IP: " + ip + ":" + port + ", ConnectedAt: " + connectedAt;
    Logger::instance().info(status);
  }
  Logger::instance().info("--- End of Status Dump ---");
}

// Start listening on specified port
bool FixServer::start(quint16 port) {
  // Init logger
  try {
    Logger::instance().init("server.log");
    ConfigManager::instance().load("server/config/fix_server.cfg");
    MessageStore::instance().init("offline_storage");
  } catch (const std::exception &e) {
    qCritical() << "Failed to initialize server: " << e.what();
    return false;
  }

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
  connectionTimeMap_[client] = QDateTime::currentDateTime();

  qInfo() << "New client connected from" << client->peerAddress().toString();

  connect(client, &QTcpSocket::readyRead, this, &FixServer::onReadyRead);
  connect(client, &QTcpSocket::disconnected, this, &FixServer::onClientDisconnected);
}

// Handle client disconnection
void FixServer::onClientDisconnected() {
  QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
  if (!client) return;

  qInfo() << "Client disconnected:" << client->peerAddress().toString();

  // --- Begin Remove from Session Map ---
  if (socketToCompIdMap_.contains(client)) {
    std::string compId = socketToCompIdMap_[client];
    socketToCompIdMap_.remove(client);
    compIdToSocketMap_.remove(compId);
    Logger::instance().info("CompID '" + compId + "' disconnected and removed from session map.");
  }
  // --- End Remove from Session Map ---

  clients_.removeAll(client);
  buffers_.remove(client);
  connectionTimeMap_.remove(client);
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

    std::string rawMsg = QString::fromUtf8(data).toStdString();
    FixMessage fixMsg = parseFixMessage(rawMsg);

    // --- Begin FIX Authentication & Routing Logic ---
    std::string senderCompId =
        FixHelper::GetTagValue(fixMsg, FixHelper::TAG_SENDER_COMP_ID);
    std::string msgType = FixHelper::GetTagValue(fixMsg, FixHelper::TAG_MSG_TYPE);

    // 1. Handle Logon
    if (msgType == "A") {
      if (!socketToCompIdMap_.contains(client)) { // Only auth new sessions
        if (!ConfigManager::instance().isCompIdAllowed(senderCompId)) {
          std::string errorMsg = "Authentication failed for CompID: " + senderCompId;
          Logger::instance().warn(errorMsg);
          client->disconnectFromHost();
          return;
        }
        std::string logMsg = "CompID '" + senderCompId + "' authenticated successfully.";
        Logger::instance().info(logMsg);
        compIdToSocketMap_[senderCompId] = client;
        socketToCompIdMap_[client] = senderCompId;

        // --- Begin Offline Message Replay ---
        Logger::instance().info("Checking for offline messages for " + senderCompId);
        std::vector<std::string> queuedMessages =
            MessageStore::instance().getAndClearQueuedMessages(senderCompId);

        if (!queuedMessages.empty()) {
          Logger::instance().info("Replaying " + std::to_string(queuedMessages.size()) + " offline messages to " + senderCompId);
          for (const auto& msgStr : queuedMessages) {
            quint32 replayPacketSize = msgStr.length();
            QByteArray replayPacket;
            replayPacket.append(reinterpret_cast<const char*>(&replayPacketSize), sizeof(replayPacketSize));
            replayPacket.append(QByteArray::fromStdString(msgStr));
            client->write(replayPacket);
          }
        }
        // --- End Offline Message Replay ---
      }
    }
    // 2. Handle Routable Messages
    else if (!senderCompId.empty()) {
      // --- Begin Message Type Validation ---
      if (!ConfigManager::instance().isMsgTypeSupported(msgType)) {
        std::string errorText = "Unsupported message type: " + msgType;
        Logger::instance().warn("Rejecting message from " + senderCompId + ". Reason: " + errorText);

        // Create Session-Level Reject message
        FixMessage rejectMsg;
        rejectMsg[8] = FixHelper::GetTagValue(fixMsg, 8); // BeginString
        rejectMsg[35] = "3"; // MsgType = Reject
        rejectMsg[49] = FixHelper::GetTagValue(fixMsg, 56); // Server's CompID
        rejectMsg[56] = senderCompId; // Original Sender's CompID
        rejectMsg[34] = "1"; // Placeholder MsgSeqNum
        rejectMsg[45] = FixHelper::GetTagValue(fixMsg, 34); // RefSeqNum
        rejectMsg[58] = errorText; // Text

        // Serialize and send back to sender
        std::string newRawMsg = serializeFixMessage(rejectMsg);
        quint32 newPacketSize = newRawMsg.length();
        QByteArray newPacket;
        newPacket.append(reinterpret_cast<const char*>(&newPacketSize), sizeof(newPacketSize));
        newPacket.append(QByteArray::fromStdString(newRawMsg));
        client->write(newPacket);
        
        return; // Stop processing this invalid message
      }
      // --- End Message Type Validation ---

      auto ruleOpt = ConfigManager::instance().getRoutingRule(senderCompId);
      if (ruleOpt) { // Routing rule found
        // Get target CompID from message
        std::string deliverToCompId =
            FixHelper::GetTagValue(fixMsg, FixHelper::TAG_DELIVER_TO_COMP_ID);

        if (!deliverToCompId.empty() && compIdToSocketMap_.contains(deliverToCompId)) {
          QTcpSocket *targetSocket = compIdToSocketMap_[deliverToCompId];

          // Add custom tags
          for (const auto &tagToAdd : ruleOpt->tagsToAdd) {
            FixHelper::SetTagValue(fixMsg, tagToAdd.tag, tagToAdd.value);
          }

          // Serialize and send
          std::string newRawMsg = serializeFixMessage(fixMsg);
          quint32 newPacketSize = newRawMsg.length();
          QByteArray newPacket;
          newPacket.append(reinterpret_cast<const char*>(&newPacketSize), sizeof(newPacketSize));
          newPacket.append(QByteArray::fromStdString(newRawMsg));
          
          targetSocket->write(newPacket);
          Logger::instance().info("Relayed message from " + senderCompId + " to " + deliverToCompId);

        } else {
          std::string warnMsg = "Routing failed: Target " + deliverToCompId + " is not online. Storing message.";
          Logger::instance().warn(warnMsg);
          MessageStore::instance().storeMessage(deliverToCompId, rawMsg);
        }
      }
    }
    // --- End FIX Authentication & Routing Logic ---
  }
}

// Parse raw string into a FixMessage map
FixMessage FixServer::parseFixMessage(const std::string &rawMsg) {
  FixMessage fixMsg;
  std::string field;
  std::istringstream stream(rawMsg);

  while (std::getline(stream, field, '\x01')) {
    size_t delimiterPos = field.find('=');
    if (delimiterPos != std::string::npos) {
      try {
        int tag = std::stoi(field.substr(0, delimiterPos));
        std::string value = field.substr(delimiterPos + 1);
        fixMsg[tag] = value;
      } catch (const std::invalid_argument& e) {
        // Ignore fields with non-integer tags
      } catch (const std::out_of_range& e) {
        // Ignore tags that are too large to be an int
      }
    }
  }
  return fixMsg;
}

// Serialize a FixMessage map back into a raw string, calculating BodyLength and CheckSum
std::string FixServer::serializeFixMessage(FixMessage &msg) {
    std::string body;
    // Build body string first to calculate its length
    for(const auto& pair : msg) {
        // BodyLength and CheckSum are not part of the body itself
        if (pair.first != 8 && pair.first != 9 && pair.first != 10) {
            body += std::to_string(pair.first) + "=" + pair.second + "\x01";
        }
    }

    // Set BodyLength
    msg[9] = std::to_string(body.length());

    // Build header string
    std::string header;
    header += "8=" + msg[8] + "\x01";
    header += "9=" + msg[9] + "\x01";

    // Calculate CheckSum
    std::string fullMessageForChecksum = header + body;
    int checksum = 0;
    for (char c : fullMessageForChecksum) {
        checksum += static_cast<int>(c);
    }
    checksum %= 256;

    // Set CheckSum (zero-padded, 3 digits)
    std::stringstream ss;
    ss << std::setw(3) << std::setfill('0') << checksum;
    msg[10] = ss.str();

    return fullMessageForChecksum + "10=" + msg[10] + "\x01";
}
