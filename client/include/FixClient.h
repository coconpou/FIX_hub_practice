#ifndef FIXCLIENT_H
#define FIXCLIENT_H

#include <map>
#include <string>

using FixMessage = std::map<int, std::string>;

class FixClient {
 public:
  FixClient(const std::string &host, int port);

  bool connectServer();
  bool sendFix(const FixMessage &msg);
  void closeConn();

 private:
  int sock_;
  std::string host_;
  int port_;

  std::string toRaw(const FixMessage &msg);
};

#endif