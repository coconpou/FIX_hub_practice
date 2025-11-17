#include "FixServer.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <quickfix/Message.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <thread>

FixServer::FixServer(FixCoder *coder, FixSessionVerity *verity, int port)
    : port_(port), listenFd_(-1), running_(false), coder_(coder), verity_(verity) {}
FixServer::~FixServer() {
  stop();
}

void FixServer::start() {
  listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (listenFd_ < 0) {
    Logger::instance().error("Failed to create socket");
    return;
  }

  int opt = 1;
  setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port_);

  if (bind(listenFd_, (sockaddr *)&addr, sizeof(addr)) < 0) {
    Logger::instance().error("Bind failed");
    return;
  }

  if (listen(listenFd_, 10) < 0) {
    Logger::instance().error("Listen failed");
    return;
  }

  running_ = true;
  std::thread(&FixServer::acceptLoop, this).detach();

  Logger::instance().info("FixServer started on port " + std::to_string(port_));
}

void FixServer::stop() {
  running_ = false;
  if (listenFd_ > 0) close(listenFd_);
}

void FixServer::acceptLoop() {
  while (running_) {
    int clientSock = accept(listenFd_, nullptr, nullptr);
    if (clientSock < 0) continue;

    Logger::instance().info("Client connected");

    std::thread(&FixServer::clientHandler, this, clientSock).detach();
  }
}

void FixServer::clientHandler(int clientSock) {
  while (true) {
    std::string raw = recvMessage(clientSock);
    if (raw.empty()) {
      Logger::instance().warn("Client disconnected.");
      break;
    }

    FixMessage msg = FixHelper::ParseRawFix(raw);

    int msgType = atoi(FixHelper::GetTagValue(msg, FixHelper::TAG_MSG_TYPE).c_str());

    // Logon (35=A)
    if (msgType == 'A') {
      if (!processLogon(msg, clientSock)) {
        Logger::instance().warn("Logon failed, closing socket");
        close(clientSock);
        return;
      }
    } else {
      processAppMessage(msg, clientSock);
    }
  }

  // Cleanup socket mapping
  if (socketToCompID.count(clientSock)) {
    std::string compID = socketToCompID[clientSock];
    socketToCompID.erase(clientSock);
    compIDToSocket.erase(compID);
  }

  close(clientSock);
}

std::string FixServer::recvMessage(int sock) {
  char buffer[4096];
  memset(buffer, 0, sizeof(buffer));

  int received = recv(sock, buffer, sizeof(buffer), 0);
  if (received <= 0) return "";

  return std::string(buffer, received);
}

void FixServer::sendMessage(int sock, const std::string &rawFix) {
  send(sock, rawFix.c_str(), rawFix.size(), 0);
}
bool FixServer::processLogon(const FixMessage &msg, int sock) {
  std::string err;
  if (!verity_->validateHeader(msg, err)) {
    Logger::instance().warn("Logon rejected: " + err);
    return false;
  }

  std::string sender = FixHelper::GetTagValue(msg, FixHelper::TAG_SENDER_COMP_ID);

  socketToCompID[sock] = sender;
  compIDToSocket[sender] = sock;

  Logger::instance().info("Logon OK for " + sender);

  flushOfflineMessages(sender, sock);
  return true;
}

void FixServer::processAppMessage(FixMessage &msg, int sock) {
  std::string err;

  if (!coder_->TransformMessage(msg, err)) {
    Logger::instance().error("Routing fail: " + err);
    return;
  }

  std::string target = FixHelper::GetTagValue(msg, FixHelper::TAG_TARGET_COMP_ID);

  if (!compIDToSocket.count(target)) {
    Logger::instance().warn("Target " + target + " offline — store message");
    MessageStore::instance().storeMessage(target, FixHelper::ToRawFix(msg));
    return;
  }

  int targetSock = compIDToSocket[target];
  sendMessage(targetSock, FixHelper::ToRawFix(msg));

  Logger::instance().info("Forwarded message to " + target);
}

void FixServer::flushOfflineMessages(const std::string &targetCompId, int sock) {
  auto msgs = MessageStore::instance().getAndClearQueuedMessages(targetCompId);

  if (msgs.empty()) return;

  Logger::instance().info("Flushing " + std::to_string(msgs.size()) +
                          " offline messages to " + targetCompId);

  for (const auto &m : msgs) {
    sendMessage(sock, m);
  }
}