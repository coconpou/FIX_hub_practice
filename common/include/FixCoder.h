#ifndef FIXCODER_H
#define FIXCODER_H

#include <map>
#include <mutex>
#include <string>

#include "ConfigManager.h"
#include "FixHelper.h"

class FixCoder {
 private:
  // Tag128 → Tag56
  std::map<std::string, std::string> deliverToTarget_;

  // Tag49 → Tag115
  std::map<std::string, std::string> senderToBehalf_;

  mutable std::mutex _mutex;

 public:
  FixCoder();
  ~FixCoder();

  // Load mapping from config.json
  void LoadFromConfig(const ConfigManager &cfg);

  // transform message to fix msg
  bool TransformMessage(FixMessage &msg, std::string &error);
};

#endif