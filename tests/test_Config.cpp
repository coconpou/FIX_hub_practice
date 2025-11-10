#include "catch.hpp"
#include "common/ConfigManager.h"
#include <fstream>
#include <unistd.h>

TEST_CASE("ConfigManager functionality", "[config]") {
    // 1. Create a temporary config file
    const char* test_config_path = "test_config.json";
    std::ofstream test_config(test_config_path);
    test_config << R"({
        "allowed_comp_ids": ["TEST_CLIENT_1", "TEST_CLIENT_2"],
        "supported_msg_types": ["D", "8"],
        "routing_rules": [
            {
                "source_comp_id": "TEST_CLIENT_1",
                "add_tags": [
                    { "tag": 9001, "value": "Special" }
                ]
            }
        ]
    })";
    test_config.close();

    // 2. Load the config
    ConfigManager::instance().load(test_config_path);

    SECTION("Allowed CompIDs are checked correctly") {
        REQUIRE(ConfigManager::instance().isCompIdAllowed("TEST_CLIENT_1") == true);
        REQUIRE(ConfigManager::instance().isCompIdAllowed("TEST_CLIENT_2") == true);
        REQUIRE(ConfigManager::instance().isCompIdAllowed("UNKNOWN_CLIENT") == false);
    }

    SECTION("Supported message types are checked correctly") {
        REQUIRE(ConfigManager::instance().isMsgTypeSupported("D") == true);
        REQUIRE(ConfigManager::instance().isMsgTypeSupported("8") == true);
        REQUIRE(ConfigManager::instance().isMsgTypeSupported("G") == false);
    }

    SECTION("Routing rules are loaded correctly") {
        auto rule = ConfigManager::instance().getRoutingRule("TEST_CLIENT_1");
        REQUIRE(rule.has_value());
        REQUIRE(rule->sourceCompId == "TEST_CLIENT_1");
        REQUIRE(rule->tagsToAdd.size() == 1);
        REQUIRE(rule->tagsToAdd[0].tag == 9001);
        REQUIRE(rule->tagsToAdd[0].value == "Special");

        auto no_rule = ConfigManager::instance().getRoutingRule("TEST_CLIENT_2");
        REQUIRE_FALSE(no_rule.has_value());
    }

    // 3. Clean up the temporary file
    unlink(test_config_path);
}
