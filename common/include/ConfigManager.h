#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "../nlohmann/json.hpp"

using json = nlohmann::json;

struct RoutingRule {
  std::string deliverId;      // Tag 128
  std::string targetCompId;   // Tag 56
};

struct OnBehalfRule {
  std::string senderCompId;   // Tag 49
  std::string onBehalfId;     // Tag 115
};

class ConfigManager {
 public:
  static ConfigManager &instance();

  void load(const std::string &configFile);

  std::optional<std::string> lookupTarget(const std::string &deliverId) const;
  std::optional<std::string> lookupBehalf(const std::string &sender) const;

  const std::vector<RoutingRule> &routingRules() const { return routingRules_; }
  const std::vector<OnBehalfRule> &behalfRules() const { return behalfRules_; }

 private:
  ConfigManager() = default;

  json configData_;

  std::vector<RoutingRule> routingRules_;
  std::vector<OnBehalfRule> behalfRules_;
};

#endif