#ifndef JSON_TLV_NULL_HPP
#define JSON_TLV_NULL_HPP

#include <cstddef>

#include <nlohmann/json.hpp>

#include "json_tlv_object.hpp"

class JsonTLVNull: public JsonTLVValueHolder<std::nullptr_t>
{
public:

    ByteArray serialize() const override;

    void deserialize(const ByteArray & bytes) override;

    void deserialize(ByteArrayIterator start, ByteArrayIterator end) override;

    Tag getTag() const override;

    nlohmann::json toJson() const override;
};

#endif // JSON_TLV_NULL_HPP
