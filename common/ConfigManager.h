#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <optional>
#include <set>
#include <string>
#include <vector>

#include "json.hpp"

// Use nlohmann::json
using json = nlohmann::json;

// Custom tag definition
struct CustomTag {
  int tag;
  std::string value;
};

// Routing rule definition
struct RoutingRule {
  std::string sourceCompId;
  std::vector<CustomTag> tagsToAdd;
};

// Configuration manager singleton
class ConfigManager {
 public:
  // Get singleton instance
  static ConfigManager &instance();

  // Delete copy constructor and assignment
  ConfigManager(const ConfigManager &) = delete;
  ConfigManager &operator=(const ConfigManager &) = delete;

  // Load configuration file
  void load(const std::string &configFile);

  // Check if CompID is allowed
  bool isCompIdAllowed(const std::string &compId) const;

  // Check if message type is supported
  bool isMsgTypeSupported(const std::string &msgType) const;

  // Get routing rule by source CompID
  std::optional<RoutingRule> getRoutingRule(const std::string &sourceCompId) const;

 private:
  // Private constructor
  ConfigManager() = default;

  json configData_;                           // JSON configuration data
  std::set<std::string> allowedCompIds_;      // Allowed CompIDs
  std::set<std::string> supportedMsgTypes_;   // Supported message types
  std::vector<RoutingRule> routingRules_;     // Routing rules
};

#endif   // CONFIG_MANAGER_H