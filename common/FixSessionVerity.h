#ifndef FIXSESSIONVERITY_H
#define FIXSESSIONVERITY_H

#include <quickfix/Message.h>

#include <QString>
#include <unordered_map>

/* Validates FIX session header fields: 49 (SenderCompID), 56 (TargetCompID), 115 (OnBehalfOfCompID), 128 (DeliverToCompID). */
class FixSessionVerity {
 public:
  FixSessionVerity();   // Constructor

  void addAllowedPair(const std::string &sender, const std::string &target);   // Register an allowed Sender/Target pair

  bool validate(const FIX::Message &msg, QString &errorMsg) const;   // Validate FIX message header fields

 private:
  std::unordered_map<std::string, std::string> allowedPairs_;   // Mapping of allowed Sender/Target pairs
};

#endif   // FIXSESSIONVERITY_H