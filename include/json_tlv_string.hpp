#ifndef JSON_TLV_STRING_HPP
#define JSON_TLV_STRING_HPP

#include <string>

#include <nlohmann/json.hpp>

#include "binary_stream.hpp"
#include "json_tlv_object.hpp"

class JsonTLVString: public JsonTLVValueHolder<std::string>
{
public:

    using JsonTLVValueHolder::JsonTLVValueHolder;

    ByteArray serialize() const override;

    void deserialize(const ByteArray & bytes) override;

    void deserialize(ByteArrayIterator start, ByteArrayIterator end) override;

    Tag getTag() const override;

    nlohmann::json toJson() const override;
};

#endif // JSON_TLV_STRING_HPP
