#ifndef FIXSESSIONVERITY_H
#define FIXSESSIONVERITY_H

#include <quickfix/Exceptions.h>   // FIX::RejectLogon
#include <quickfix/Field.h>
#include <quickfix/FieldNumbers.h>
#include <quickfix/Message.h>
#include <quickfix/Session.h>
#include <quickfix/SessionSettings.h>

#include <string>
#include <unordered_map>

// Session header validator for FIX protocol
class FixSessionVerity {
 public:
  FixSessionVerity();
  ~FixSessionVerity();

  // Add a single allowed Sender/Target pair
  void addAllowedPair(const std::string &sender, const std::string &target);

  // Replace all allowed pairs at once
  void setAllowedPairs(const std::unordered_map<std::string, std::string> &pairs);

  // Validate FIX header fields 49/56
  bool validateHeader(const FIX::Message &msg, std::string &errorMsg) const;

  // Validate Logon message (MsgType=A), throw RejectLogon if invalid
  void validateLogonOrThrow(const FIX::Message &logonMsg) const;

 private:
  std::unordered_map<std::string, std::string> allowedPairs_;                          // Sender(49) → Target(56)
  static bool tryGetHeaderField(const FIX::Message &msg, int tag, std::string &out);   // Helper to read tag
};

#endif   // FIXSESSIONVERITY_H