#include <vector>
#include <utility>

#include <catch2/catch.hpp>

#include "json_tlv_integral.hpp"
#include "json_tlv_object.hpp"

template<class T>
class TestJsonTLVIntegral: public T
{
public:
    using T::T;
    using T::serializeValue;
    using T::deserializeValue;
    using T::getTag;
    using T::Tag;
    // using T::operator T::ValueType;
};

using TestJsonTLVInt = TestJsonTLVIntegral<JsonTLVInt>;
using TestJsonTLVBoolean = TestJsonTLVIntegral<JsonTLVBoolean>;

TEST_CASE("JSON TLV integral value", "[tlv-integral]")
{
    SECTION("TLV Integer values")
    {
        SECTION("Tag should be integer")
        {
            REQUIRE(TestJsonTLVInt().getTag() == TestJsonTLVInt::Tag::Integer);
        }

        auto test_pair = GENERATE(
            std::make_pair(TestJsonTLVInt(1), std::vector<uint8_t>{0x01}),
            std::make_pair(TestJsonTLVInt(0xFF), std::vector<uint8_t>{0xFF}),
            std::make_pair(TestJsonTLVInt(0b11100100101011110101010010101011),
                           std::vector<uint8_t>{0xAB, 0x54, 0xAF, 0xE4}));

        SECTION("Should serialize integer")
        {
            auto & tlv_int = test_pair.first;
            auto & expected_serialization = test_pair.second;

            REQUIRE_THAT(tlv_int.serializeValue(),
                        Catch::Matchers::Equals(expected_serialization));
        }

        SECTION("Should serialize 0 to empty array")
        {
            REQUIRE_THAT(TestJsonTLVInt(0).serializeValue(),
                         Catch::Matchers::Equals(std::vector<uint8_t>()));
        }

        SECTION("Should deserialize integer")
        {
            auto & expected_tlv_int = test_pair.first;
            auto & serialization = test_pair.second;

            TestJsonTLVInt deserialized_value;
            deserialized_value.deserializeValue(serialization);

            REQUIRE(deserialized_value == expected_tlv_int);
        }

        SECTION("Should deserialize an empty array to 0")
        {
            TestJsonTLVInt deserialized_value(1);
            deserialized_value.deserializeValue({});

            REQUIRE(deserialized_value == 0);
        }
    }

    SECTION("TLV boolean values")
    {
        SECTION("Tag should be boolean")
        {
            auto boolean_tag = TestJsonTLVBoolean::Tag::Boolean;
            REQUIRE(TestJsonTLVBoolean().getTag() == boolean_tag);
        }

        SECTION("Should serialize true to 0x01")
        {
            REQUIRE_THAT(TestJsonTLVBoolean(true).serializeValue(),
                         Catch::Matchers::Equals(std::vector<uint8_t>{0x01}));
        }

        SECTION("Should serialize false to empty array")
        {
            REQUIRE_THAT(TestJsonTLVBoolean(false).serializeValue(),
                         Catch::Matchers::Equals(std::vector<uint8_t>()));
        }

        SECTION("Should deserialize 0x01 to true")
        {
            TestJsonTLVBoolean deserialized_value(false);
            deserialized_value.deserializeValue({0x01});

            REQUIRE((bool) deserialized_value);
        }

        SECTION("Should deserialize an empty array to false")
        {
            TestJsonTLVBoolean deserialized_value(true);
            deserialized_value.deserializeValue({});

            REQUIRE_FALSE((bool) deserialized_value);
        }
    }
}
