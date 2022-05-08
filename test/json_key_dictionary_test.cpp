#include <exception>

#include <catch2/catch.hpp>

#include "json_key_dictionary.hpp"

TEST_CASE("JSON key dictionary", "[json-key-dict]")
{
    SECTION("New key ids should increase by 1")
    {
        JsonKeyDictionary dictionary;

        REQUIRE(dictionary.insert("key1") == 1);
        REQUIRE(dictionary.insert("key2") == 2);
        REQUIRE(dictionary.insert("key3") == 3);
    }

    SECTION("Key ids should not change")
    {
        JsonKeyDictionary dictionary;

        REQUIRE(dictionary.insert("key1") == 1);
        REQUIRE(dictionary.insert("key1") == 1);
        REQUIRE(dictionary.insert("key2") == 2);
    }

    SECTION("Should query key ids")
    {
        JsonKeyDictionary dictionary;
        int id = dictionary.insert("key1");
        int queried_id;

        REQUIRE_NOTHROW(queried_id = dictionary.get("key1"));
        REQUIRE(queried_id == id);

        REQUIRE_THROWS_AS(dictionary.get("key2"), std::out_of_range);
    }
}
