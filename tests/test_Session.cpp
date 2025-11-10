#include "catch.hpp"
#include "common/MessageStore.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

TEST_CASE("MessageStore functionality", "[store]") {
    const std::string test_storage_path = "test_storage";
    const std::string target_comp_id = "OFFLINE_CLIENT";
    const std::string message_content = "8=FIX.4.4\x01" "35=D\x01" "49=SENDER\x01";

    // Ensure the test directory is clean before we start
    if (fs::exists(test_storage_path)) {
        fs::remove_all(test_storage_path);
    }
    fs::create_directory(test_storage_path);

    // Initialize the store
    MessageStore::instance().init(test_storage_path);

    SECTION("Store a single message") {
        bool stored = MessageStore::instance().storeMessage(target_comp_id, message_content);
        REQUIRE(stored == true);

        // Verify that the file was actually created
        fs::path target_dir = fs::path(test_storage_path) / target_comp_id;
        REQUIRE(fs::exists(target_dir));
        
        int file_count = 0;
        std::string created_filename;
        for (const auto& entry : fs::directory_iterator(target_dir)) {
            file_count++;
            created_filename = entry.path().filename().string();
        }
        REQUIRE(file_count == 1);
        REQUIRE(created_filename.find(".fix") != std::string::npos);
    }

    SECTION("Retrieve and clear messages") {
        // First, store a message to retrieve
        MessageStore::instance().storeMessage(target_comp_id, message_content);
        MessageStore::instance().storeMessage(target_comp_id, message_content + "2");

        // Now, get and clear them
        std::vector<std::string> messages = MessageStore::instance().getAndClearQueuedMessages(target_comp_id);

        // Verify content
        REQUIRE(messages.size() == 2);
        REQUIRE(messages[0] == message_content);
        REQUIRE(messages[1] == message_content + "2");

        // Verify that the files were deleted
        fs::path target_dir = fs::path(test_storage_path) / target_comp_id;
        REQUIRE(fs::is_empty(target_dir));
    }

    // Clean up the test directory
    fs::remove_all(test_storage_path);
}
