#include "catch.hpp"
#include "server/src/FixServer.h" // Need access to the static serializeFixMessage

// Note: We are testing a static private member. This is generally not ideal,
// but for a critical utility function, it's a pragmatic choice.
// To make this work, we must declare it public in FixServer.h for the test build.
// A better long-term solution would be to move serializeFixMessage to FixHelper.

TEST_CASE("FIX Message Serialization", "[serialize]") {

    SECTION("Basic message serialization with BodyLength and CheckSum") {
        // 1. Create a sample message
        FixMessage msg = {
            {8, "FIX.4.4"},
            {35, "D"},
            {49, "SENDER"},
            {56, "TARGET"},
            {34, "1"},
            {11, "ORDER123"}
        };

        // 2. Manually calculate expected body
        // Body includes fields other than 8, 9, 10
        std::string body_part = "35=D\x01" "49=SENDER\x01" "56=TARGET\x01" "34=1\x01" "11=ORDER123\x01";
        size_t body_length = body_part.length();

        // 3. Manually calculate expected checksum
        std::string header_part = "8=FIX.4.4\x01" "9=" + std::to_string(body_length) + "\x01";
        std::string checksum_base = header_part + body_part;
        int checksum = 0;
        for (char c : checksum_base) {
            checksum += static_cast<int>(c);
        }
        checksum %= 256;
        
        char checksum_str[4];
        snprintf(checksum_str, 4, "%03d", checksum);

        // 4. Construct the full expected message
        std::string expected_message = checksum_base + "10=" + checksum_str + "\x01";

        // 5. Call the actual function
        // The function modifies the map, so we pass a copy
        FixMessage msg_to_serialize = msg;
        std::string serialized_message = FixServer::serializeFixMessage(msg_to_serialize);

        // 6. Verify
        REQUIRE(serialized_message == expected_message);
        REQUIRE(msg_to_serialize[9] == std::to_string(body_length));
        REQUIRE(msg_to_serialize[10] == checksum_str);
    }
}
