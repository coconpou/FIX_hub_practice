#include "ConfigManager.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

ConfigManager &ConfigManager::instance() {
  static ConfigManager inst;
  return inst;
}

void ConfigManager::load(const string &configFile) {
  ifstream file(configFile);
  if (!file.is_open()) {
    throw runtime_error("Failed to open config file: " + configFile);
  }

  try {
    configData_ = json::parse(file);

    // ===== Load routing_rules =====
    if (configData_.contains("routing_rules")) {
      routingRules_.clear();

      for (auto &ruleJson : configData_["routing_rules"]) {
        RoutingRule rule;

        if (ruleJson.contains("deliver_id"))
          rule.deliverId = ruleJson["deliver_id"].get<string>();
        else
          continue;

        if (ruleJson.contains("target_comp_id"))
          rule.targetCompId = ruleJson["target_comp_id"].get<string>();
        else
          continue;

        routingRules_.push_back(rule);
      }
    }

    // ===== Load on_behalf_rules =====
    if (configData_.contains("on_behalf_rules")) {
      behalfRules_.clear();

      for (auto &r : configData_["on_behalf_rules"]) {
        OnBehalfRule rule;

        if (r.contains("sender_comp_id"))
          rule.senderCompId = r["sender_comp_id"].get<string>();
        else
          continue;

        if (r.contains("on_behalf_id"))
          rule.onBehalfId = r["on_behalf_id"].get<string>();
        else
          continue;

        behalfRules_.push_back(rule);
      }
    }

  } catch (const json::exception &e) {
    throw runtime_error("Failed to parse config.json: " + string(e.what()));
  }
}

optional<string> ConfigManager::lookupTarget(const string &deliverId) const {
  for (const auto &r : routingRules_) {
    if (r.deliverId == deliverId) return r.targetCompId;
  }
  return nullopt;
}

optional<string> ConfigManager::lookupBehalf(const string &sender) const {
  for (const auto &r : behalfRules_) {
    if (r.senderCompId == sender) return r.onBehalfId;
  }
  return nullopt;
}