#ifndef JSON_TLV_FLOAT_HPP
#define JSON_TLV_FLOAT_HPP

#include <nlohmann/json.hpp>

#include "json_tlv_object.hpp"
#include "binary_stream.hpp"

class JsonTLVFloat: public JsonTLVValueHolder<long double>
{
public:

    using JsonTLVValueHolder::JsonTLVValueHolder;

    ByteArray serialize() const override;

    void deserialize(const ByteArray & bytes) override;

    void deserialize(ByteArrayIterator start, ByteArrayIterator end) override;

    Tag getTag() const override;

    nlohmann::json toJson() const override;
};

#endif // JSON_TLV_FLOAT_HPP
