#include "FixClient.h"

#include <arpa/inet.h>
#include <unistd.h>

#include "FixHelper.h"

FixClient::FixClient(const std::string &host, int port)
    : sock_(-1), host_(host), port_(port) {}

bool FixClient::connectServer() {
  sock_ = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  inet_pton(AF_INET, host_.c_str(), &addr.sin_addr);
  addr.sin_port = htons(port_);

  return connect(sock_, (sockaddr *)&addr, sizeof(addr)) == 0;
}

bool FixClient::sendFix(const FixMessage &msg) {
  std::string raw = FixHelper::ToRawFix(msg);
  return write(sock_, raw.c_str(), raw.size()) == (int)raw.size();
}

void FixClient::closeConn() {
  if (sock_ >= 0) close(sock_);
}