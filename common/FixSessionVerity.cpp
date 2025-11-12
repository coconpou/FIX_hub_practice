#include "FixSessionVerity.h"

#include <iostream>

using namespace std;

// Constructor
FixSessionVerity::FixSessionVerity() {}

// Destructor
FixSessionVerity::~FixSessionVerity() = default;

// Add a single allowed Sender/Target pair
void FixSessionVerity::addAllowedPair(const string &sender, const string &target) {
  allowedPairs_[sender] = target;
}

// Replace all allowed pairs at once
void FixSessionVerity::setAllowedPairs(const unordered_map<string, string> &pairs) {
  allowedPairs_ = pairs;
}

// Try to extract a header field by tag
bool FixSessionVerity::tryGetHeaderField(const FIX::Message &msg, int tag, string &out) {
  if (!msg.getHeader().isSetField(tag)) return false;
  FIX::StringField f(tag);
  msg.getHeader().getField(f);
  out = f.getString();
  return true;
}

// Validate FIX header: check SenderCompID and TargetCompID
bool FixSessionVerity::validateHeader(const FIX::Message &msg, string &errorMsg) const {
  string sender, target;

  if (!tryGetHeaderField(msg, FIX::FIELD::SenderCompID, sender) ||
      !tryGetHeaderField(msg, FIX::FIELD::TargetCompID, target)) {
    errorMsg = "Missing SenderCompID(49) or TargetCompID(56)";
    return false;
  }

  auto it = allowedPairs_.find(sender);
  if (it == allowedPairs_.end()) {
    errorMsg = "Unauthorized SenderCompID: " + sender;
    return false;
  }

  if (it->second != target) {
    errorMsg = "TargetCompID mismatch: expected " + it->second + " but got " + target;
    return false;
  }

  // Optional logging for fields 115 and 128
  string behalf, deliver;
  if (tryGetHeaderField(msg, FIX::FIELD::OnBehalfOfCompID, behalf)) {
    cout << "[FIX] OnBehalfOfCompID(115)=" << behalf << endl;
  }
  if (tryGetHeaderField(msg, FIX::FIELD::DeliverToCompID, deliver)) {
    cout << "[FIX] DeliverToCompID(128)=" << deliver << endl;
  }

  return true;
}

// Validate Logon (MsgType=A), throw RejectLogon if invalid
void FixSessionVerity::validateLogonOrThrow(const FIX::Message &logonMsg) const {
  string msgType;
  if (!tryGetHeaderField(logonMsg, FIX::FIELD::MsgType, msgType) || msgType != "A") {
    throw FIX::RejectLogon("Invalid call: validateLogonOrThrow expects MsgType=A (Logon).");
  }

  string err;
  if (!validateHeader(logonMsg, err)) {
    throw FIX::RejectLogon(err);
  }
}