#include "FixSessionVerity.h"

#include <iostream>

FixSessionVerity::FixSessionVerity() {}

FixSessionVerity::~FixSessionVerity() = default;

void FixSessionVerity::addAllowedPair(const std::string &sender, const std::string &target) {
  allowedPairs_[sender] = target;
}

void FixSessionVerity::setAllowedPairs(const std::unordered_map<std::string, std::string> &pairs) {
  allowedPairs_ = pairs;
}

// Extract value from FixMessage (std::map<int,string>)
bool FixSessionVerity::tryGetHeaderField(const FixMessage &msg, int tag, std::string &out) const {
  auto it = msg.find(tag);
  if (it == msg.end()) return false;
  out = it->second;
  return true;
}

bool FixSessionVerity::validateHeader(const FixMessage &msg, std::string &err) const {
  std::string sender, target;

  // Must have SenderCompID(49) and TargetCompID(56)
  if (!tryGetHeaderField(msg, 49, sender) ||
      !tryGetHeaderField(msg, 56, target)) {
    err = "Missing SenderCompID(49) or TargetCompID(56)";
    return false;
  }

  // Sender must be in allowedPairs
  auto it = allowedPairs_.find(sender);
  if (it == allowedPairs_.end()) {
    err = "Unauthorized SenderCompID: " + sender;
    return false;
  }

  // Target must match expected target
  if (it->second != target) {
    err = "TargetCompID mismatch: expected " + it->second +
          " but got " + target;
    return false;
  }

  // Optional logging for 115 / 128
  std::string behalf, deliver;
  if (tryGetHeaderField(msg, 115, behalf)) {
    std::cout << "[FIX] OnBehalfOfCompID(115)=" << behalf << std::endl;
  }
  if (tryGetHeaderField(msg, 128, deliver)) {
    std::cout << "[FIX] DeliverToCompID(128)=" << deliver << std::endl;
  }

  return true;
}

void FixSessionVerity::validateLogonOrThrow(const FixMessage &msg) const {
  std::string msgType;

  if (!tryGetHeaderField(msg, 35, msgType) || msgType != "A") {
    throw std::runtime_error("Logon must have MsgType=A");
  }

  std::string err;
  if (!validateHeader(msg, err)) {
    throw std::runtime_error("Logon validation failed: " + err);
  }
}