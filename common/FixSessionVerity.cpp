#include "FixSessionVerity.h"

#include <quickfix/Field.h>
#include <quickfix/fix44/Logon.h>

#include <QDebug>

using namespace std;

// Constructor
FixSessionVerity::FixSessionVerity() {}

// Destructor
FixSessionVerity::~FixSessionVerity() = default;

// Register allowed Sender/Target pair
void FixSessionVerity::addAllowedPair(const string &sender, const string &target) {
  allowedPairs_[sender] = target;
}

// Validate FIX message header fields
bool FixSessionVerity::validate(const FIX::Message &msg, QString &errorMsg) const {
  FIX::SenderCompID sender;
  FIX::TargetCompID target;

  if (!msg.getHeader().isSetField(FIX::FIELD::SenderCompID) || !msg.getHeader().isSetField(FIX::FIELD::TargetCompID)) {
    errorMsg = "Missing SenderCompID or TargetCompID";
    return false;
  }

  msg.getHeader().getField(sender);
  msg.getHeader().getField(target);

  auto it = allowedPairs_.find(sender.getValue());
  if (it == allowedPairs_.end()) {
    errorMsg = QString("Unauthorized SenderCompID: %1").arg(QString::fromStdString(sender.getValue()));
    return false;
  }

  if (it->second != target.getValue()) {
    errorMsg = QString("TargetCompID mismatch: expected %1 but got %2")
                   .arg(QString::fromStdString(it->second))
                   .arg(QString::fromStdString(target.getValue()));
    return false;
  }

  if (msg.getHeader().isSetField(FIX::FIELD::OnBehalfOfCompID)) {
    FIX::OnBehalfOfCompID behalf;
    msg.getHeader().getField(behalf);
    qInfo() << "[FIX]" << "OnBehalfOfCompID =" << QString::fromStdString(behalf.getValue());
  }

  if (msg.getHeader().isSetField(FIX::FIELD::DeliverToCompID)) {
    FIX::DeliverToCompID deliver;
    msg.getHeader().getField(deliver);
    qInfo() << "[FIX]" << "DeliverToCompID =" << QString::fromStdString(deliver.getValue());
  }

  return true;
}