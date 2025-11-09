#ifndef FIXSESSIONVERITY_H
#define FIXSESSIONVERITY_H

#include <quickfix/Message.h>

#include <QString>
#include <unordered_map>

// Validates FIX session header fields: 49 (SenderCompID), 56 (TargetCompID), 115 (OnBehalfOfCompID), 128 (DeliverToCompID)
class FixSessionVerity {
 public:
  // Constructor
  FixSessionVerity();

  // Destructor
  ~FixSessionVerity();

  // Register allowed Sender/Target pair
  void addAllowedPair(const std::string &sender, const std::string &target);

  // Validate FIX message header fields
  bool validate(const FIX::Message &msg, QString &errorMsg) const;

 private:
  // Allowed Sender/Target pairs
  std::unordered_map<std::string, std::string> allowedPairs_;
};

#endif   // FIXSESSIONVERITY_H