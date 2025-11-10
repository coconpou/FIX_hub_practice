#include "ConfigManager.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

// Get singleton instance
ConfigManager &ConfigManager::instance() {
  static ConfigManager instance;
  return instance;
}

// Load configuration file
void ConfigManager::load(const string &configFile) {
  ifstream file(configFile);
  if (!file.is_open()) {
    throw runtime_error("Failed to open config file: " + configFile);
  }

  try {
    configData_ = json::parse(file);

    // Load allowed CompIDs
    if (configData_.contains("allowed_comp_ids")) {
      for (const auto &id : configData_["allowed_comp_ids"]) {
        allowedCompIds_.insert(id.get<string>());
      }
    }

    // Load supported message types
    if (configData_.contains("supported_msg_types")) {
      for (const auto &type : configData_["supported_msg_types"]) {
        supportedMsgTypes_.insert(type.get<string>());
      }
    }

    // Load routing rules
    if (configData_.contains("routing_rules")) {
      for (const auto &ruleJson : configData_["routing_rules"]) {
        RoutingRule rule;
        rule.sourceCompId = ruleJson["source_comp_id"].get<string>();

        if (ruleJson.contains("add_tags")) {
          for (const auto &tagJson : ruleJson["add_tags"]) {
            CustomTag tag;
            tag.tag = tagJson["tag"].get<int>();
            tag.value = tagJson["value"].get<string>();
            rule.tagsToAdd.push_back(tag);
          }
        }
        routingRules_.push_back(rule);
      }
    }
  } catch (const json::parse_error &e) {
    throw runtime_error("Failed to parse config file: " + string(e.what()));
  }
}

// Load allowed CompIDs from a separate file
void ConfigManager::loadAllowedCompIds(const string &filePath) {
  ifstream file(filePath);
  if (!file.is_open()) {
    throw runtime_error("Failed to open allowed CompIDs file: " + filePath);
  }

  try {
    json compIdData;
    file >> compIdData;

    if (compIdData.contains("allowed_comp_ids")) {
      for (const auto &id : compIdData["allowed_comp_ids"]) {
        allowedCompIds_.insert(id.get<string>());
      }
    }
  } catch (const json::parse_error &e) {
    throw runtime_error("Failed to parse allowed CompIDs file: " + string(e.what()));
  }
}

// Check if CompID is allowed
bool ConfigManager::isCompIdAllowed(const string &compId) const {
  return allowedCompIds_.count(compId) > 0;
}

// Check if message type is supported
bool ConfigManager::isMsgTypeSupported(const string &msgType) const {
  // If no types specified, allow all by default
  if (supportedMsgTypes_.empty()) return true;
  return supportedMsgTypes_.count(msgType) > 0;
}

// Get routing rule by source CompID
optional<RoutingRule> ConfigManager::getRoutingRule(const string &sourceCompId) const {
  for (const auto &rule : routingRules_) {
    if (rule.sourceCompId == sourceCompId) {
      return rule;
    }
  }
  return nullopt;
}