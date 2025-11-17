#ifndef FIXSERVER_H
#define FIXSERVER_H

#include <atomic>
#include <string>
#include <unordered_map>

#include "FixCoder.h"
#include "FixHelper.h"
#include "FixSessionVerity.h"
#include "Logger.h"
#include "MessageStore.h"

class FixServer {
 public:
  FixServer(FixCoder *coder, FixSessionVerity *verity, int port);
  ~FixServer();

  void start();
  void stop();

 private:
  int port_;
  int listenFd_;
  std::atomic<bool> running_;

  FixCoder *coder_;
  FixSessionVerity *verity_;

  // session maps
  std::unordered_map<int, std::string> socketToCompID;
  std::unordered_map<std::string, int> compIDToSocket;

 private:
  void acceptLoop();
  void clientHandler(int clientSock);

  std::string recvMessage(int sock);
  void sendMessage(int sock, const std::string &rawFix);

  bool processLogon(const FixMessage &msg, int sock);
  void processAppMessage(FixMessage &msg, int sock);

  void flushOfflineMessages(const std::string &targetCompId, int sock);
};

#endif