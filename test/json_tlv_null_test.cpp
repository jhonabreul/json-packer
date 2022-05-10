#include <vector>
#include <utility>
#include <memory>

#include <catch2/catch.hpp>

#include "json_tlv_null.hpp"
#include "json_tlv_object.hpp"

TEST_CASE("JSON TLV null", "[tlv-null]")
{
    SECTION("Tag should be 'Null'")
    {
        REQUIRE(JsonTLVNull().getTag() == JsonTLVObject::Tag::Null);
    }
    SECTION("Should always be null")
    {
        REQUIRE(static_cast<JsonTLVNull::ValueType>(JsonTLVNull()) == nullptr);
    }

    SECTION("Should always serialize to empty array")
    {
        REQUIRE_THAT(JsonTLVNull().serialize(),
                     Catch::Matchers::Equals(std::vector<uint8_t>()));
    }

    SECTION("Should always deserialize from empty array")
    {
        JsonTLVNull deserialized_value;
        REQUIRE_NOTHROW(deserialized_value.deserialize({}));
    }

    // TODO: This should throw
    /*
    SECTION("Should not deserialize from non-empty array")
    {
        JsonTLVNull deserialized_value;
        REQUIRE_THROWS(deserialized_value.deserialize({0x00}));
    }
    */
}
