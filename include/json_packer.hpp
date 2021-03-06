#ifndef JSON_PACKER_HPP
#define JSON_PACKER_HPP

#include <map>
#include <vector>
#include <iostream>
#include <cstdint>
#include <string>
#include <memory>

#include <nlohmann/json.hpp>

#include "binary_stream.hpp"
#include "json_key_dictionary.hpp"
#include "json_tlv_object.hpp"
#include "json_tlv_record.hpp"

class JsonPacker
{
public:

    static void pack(std::istream & in, std::ostream & out);

    static ByteArray packLine(const std::string & line,
                              JsonKeyDictionary & dictionary);

    static void unpack(std::istream & in, std::ostream & out);

    static std::shared_ptr<JsonTLVObject> unpackLine(ByteArrayIterator start,
                                                     ByteArrayIterator end);
};

#endif // JSON_PACKER_HPP
