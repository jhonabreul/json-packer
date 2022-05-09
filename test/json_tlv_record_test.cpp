#include <vector>
#include <utility>
#include <memory>

#include <catch2/catch.hpp>

#include "json_tlv_record.hpp"
#include "json_tlv_object.hpp"

TEST_CASE("JSON TLV record", "[tlv-record]")
{
    SECTION("Tag should be 'Record'")
    {
        REQUIRE(JsonTLVRecord().getTag() == JsonTLVInt::Tag::Record);

        SECTION("Should serialize {} to empty array")
        {
            REQUIRE_THAT(JsonTLVRecord().serialize(),
                         Catch::Matchers::Equals(std::vector<uint8_t>()));
        }

        SECTION("Should deserialize empty array to {}")
        {
            JsonTLVRecord deserialized_value({
                {1, std::make_shared<JsonTLVBoolean>()}
            });
            REQUIRE_FALSE(
                ((JsonTLVRecord::ValueType) deserialized_value).empty());

            deserialized_value.deserialize({});
            REQUIRE(((JsonTLVRecord::ValueType) deserialized_value).empty());
        }

        auto test_pair = GENERATE(
            std::make_pair(JsonTLVRecord({
                {1, std::make_shared<JsonTLVBoolean>(true)},
                {0xF0FF, std::make_shared<JsonTLVBoolean>(false)},
                {0xABCDEF, std::make_shared<JsonTLVInt>(0)},
                {0xABCDEF01, std::make_shared<JsonTLVInt>(0x1A2B3C4D5E)},
            }), std::vector<uint8_t>{
                // START: Key-value pair 1
                    // START: Key (int) 1
                    0x21, // Tag-length byte
                    0x01,  // Value byte
                    // END: Key (int) 1
                    // START: Boolean true
                    0x61, // Tag-length byte
                    0x01,  // Value byte
                    // END: Boolean true
                // END: Key-value pair 1
                // START: Key-value pair 2
                    // START: Key (int) 0xF0FF
                    0x22, // Tag-length byte
                    0xFF,  // Value byte 1
                    0xF0,  // Value byte 2
                    // END: Key (int) 0xF0FF
                    // START: Boolean false
                    0x60, // Tag-length byte
                    // No value byte
                    // END: Boolean false
                // END: Key-value pair 2
                // START: Key-value pair 3
                    // START: Key (int) 0xABCDEF
                    0x23, // Tag-length byte
                    0xEF,  // Value byte 1
                    0xCD,  // Value byte 2
                    0xAB,  // Value byte 3
                    // END: Key (int) 0xABCDEF
                    // START: Int 0
                    0x20, // Tag-length byte
                    // No value byte
                    // END: Int 0
                // END: Key-value pair 3
                // START: Key-value pair 4
                    // START: Key (int) 0xABCDEF01
                    0x24, // Tag-length byte
                    0x01,  // Value byte 1
                    0xEF,  // Value byte 2
                    0xCD,  // Value byte 3
                    0xAB,  // Value byte 4
                    // END: Key (int) 0xABCDEF01
                    // START: Int 0x1A2B3C4D5E
                    0x25, // Tag-length byte
                    0x5E,  // Value byte 1
                    0x4D,  // Value byte 2
                    0x3C,  // Value byte 3
                    0x2B,  // Value byte 4
                    0x1A,  // Value byte 5
                    // END: Int 0x1A2B3C4D5E
                // END: Key-value pair 4
            })
        );

        SECTION("Should serialize record")
        {
            auto & tlv_record = test_pair.first;
            auto & expected_serialization = test_pair.second;

            REQUIRE_THAT(tlv_record.serialize(),
                         Catch::Matchers::Equals(expected_serialization));
        }

        SECTION("Should deserialize record")
        {
            auto & expected_tlv_record = test_pair.first;
            auto & serialization = test_pair.second;

            JsonTLVRecord deserialized_value;
            deserialized_value.deserialize(serialization);

            REQUIRE(deserialized_value.equalTo(expected_tlv_record));
        }
    }
}
