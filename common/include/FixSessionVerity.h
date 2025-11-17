#ifndef FIXSESSIONVERITY_H
#define FIXSESSIONVERITY_H

#include <string>
#include <unordered_map>

#include "FixHelper.h"   // for FixMessage

class FixSessionVerity {
 public:
  FixSessionVerity();
  ~FixSessionVerity();

  // Add one allowed sender/target pair
  void addAllowedPair(const std::string &sender, const std::string &target);

  // Replace allowed pairs at once
  void setAllowedPairs(const std::unordered_map<std::string, std::string> &pairs);

  // Validate header fields 49/56
  bool validateHeader(const FixMessage &msg, std::string &errorMsg) const;

  // Validate Logon (MsgType = A)
  void validateLogonOrThrow(const FixMessage &msg) const;

 private:
  std::unordered_map<std::string, std::string> allowedPairs_;   // Sender → Target mapping

  // Extract tag value from FixMessage
  bool tryGetHeaderField(const FixMessage &msg, int tag, std::string &out) const;
};

#endif