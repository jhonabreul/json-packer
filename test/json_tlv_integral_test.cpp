#include <vector>
#include <utility>

#include <catch2/catch.hpp>

#include "json_tlv_integral.hpp"
#include "json_tlv_object.hpp"

TEST_CASE("JSON TLV integral value", "[tlv-integral]")
{
    SECTION("TLV Integer values")
    {
        SECTION("Tag should be integer")
        {
            REQUIRE(JsonTLVInt().getTag() == JsonTLVInt::Tag::Integer);
        }

        auto test_pair = GENERATE(
            std::make_pair(JsonTLVInt(1), std::vector<uint8_t>{0x01}),
            std::make_pair(JsonTLVInt(0xFF), std::vector<uint8_t>{0xFF}),
            std::make_pair(JsonTLVInt(0b11100100101011110101010010101011),
                           std::vector<uint8_t>{0xAB, 0x54, 0xAF, 0xE4}));

        SECTION("Should serialize integer")
        {
            auto & tlv_int = test_pair.first;
            auto & expected_serialization = test_pair.second;

            REQUIRE_THAT(tlv_int.serialize(),
                         Catch::Matchers::Equals(expected_serialization));
        }

        SECTION("Should serialize 0 to empty array")
        {
            REQUIRE_THAT(JsonTLVInt(0).serialize(),
                         Catch::Matchers::Equals(std::vector<uint8_t>()));
        }

        SECTION("Should deserialize integer")
        {
            auto & expected_tlv_int = test_pair.first;
            auto & serialization = test_pair.second;

            JsonTLVInt deserialized_value;
            deserialized_value.deserialize(serialization);

            REQUIRE(deserialized_value == expected_tlv_int);
        }

        SECTION("Should deserialize an empty array to 0")
        {
            JsonTLVInt deserialized_value(1);
            deserialized_value.deserialize({});

            REQUIRE(deserialized_value == 0);
        }
    }

    SECTION("TLV boolean values")
    {
        SECTION("Tag should be boolean")
        {
            auto boolean_tag = JsonTLVBoolean::Tag::Boolean;
            REQUIRE(JsonTLVBoolean().getTag() == boolean_tag);
        }

        SECTION("Should serialize true to 0x01")
        {
            REQUIRE_THAT(JsonTLVBoolean(true).serialize(),
                         Catch::Matchers::Equals(std::vector<uint8_t>{0x01}));
        }

        SECTION("Should serialize false to empty array")
        {
            REQUIRE_THAT(JsonTLVBoolean(false).serialize(),
                         Catch::Matchers::Equals(std::vector<uint8_t>()));
        }

        SECTION("Should deserialize 0x01 to true")
        {
            JsonTLVBoolean deserialized_value(false);
            deserialized_value.deserialize({0x01});

            REQUIRE((bool) deserialized_value);
        }

        SECTION("Should deserialize an empty array to false")
        {
            JsonTLVBoolean deserialized_value(true);
            deserialized_value.deserialize({});

            REQUIRE_FALSE((bool) deserialized_value);
        }
    }
}
