#include "FixClient.h"
#include "FixHelper.h"

int main() {
  FixClient cli("127.0.0.1", 5001);
  cli.connectServer();

  FixMessage logon;
  logon[35] = "A";
  logon[49] = "ABC";
  logon[56] = "HUB";
  cli.sendFix(logon);

  FixMessage order;
  order[35] = "D";
  order[49] = "ABC";
  order[56] = "HUB";
  order[128] = "BR1";
  order[11] = "ORDER1";

  cli.sendFix(order);

  cli.closeConn();
}