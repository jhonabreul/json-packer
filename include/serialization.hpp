#ifndef SERIALIZATION_HPP
#define SERIALIZATION_HPP

#include <vector>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <memory>
#include <tuple>
#include <exception>

#include "json_tlv_object.hpp"
#include "binary_stream.hpp"

#define INTEGRAL_TYPE_TEMPLATE \
    template< \
        class T, \
        typename = typename std::enable_if_t<std::is_integral<T>::value, T> \
    >

INTEGRAL_TYPE_TEMPLATE
std::vector<uint8_t> serializeIntegralValue(T value)
{
    constexpr auto type_size = sizeof(T);
    std::vector<uint8_t> bytes(type_size);

    for (int i = 0; i < type_size; i++) {
        bytes[i] = (value >> (i * 8)) & 0xFF;
    }

    auto size = type_size;

    while (size > 1 && bytes[size - 1] == 0) {
        size--;
    }

    bytes.resize(size);

    return bytes;
}

INTEGRAL_TYPE_TEMPLATE
T deserializeIntegralValue(std::vector<uint8_t>::const_iterator start,
                           std::vector<uint8_t>::const_iterator end)
{
    if (end - start > sizeof(T)) {
        throw std::out_of_range(
            "Too many bytes for the type beign deserialized");
    }

    T value = 0;
    for (int i = 0; start != end; i++) {
        value |= (T) *start++ << (i * 8);
    }

    return value;
}

INTEGRAL_TYPE_TEMPLATE
T deserializeIntegralValue(const std::vector<uint8_t> & bytes)
{
    return deserializeIntegralValue<T>(bytes.begin(), bytes.end());
}

ByteArray serializeTLVElement(const JsonTLVObject & tlv_value);

void serializeTLVElement(const JsonTLVObject & tlv_value,
                         BinaryOutputStream & out);

std::pair<std::shared_ptr<JsonTLVObject>, ByteArrayIterator>
deserializeTLVElement(ByteArrayIterator start, ByteArrayIterator end);

std::shared_ptr<JsonTLVObject> deserializeTLVElement(BinaryInputStream & in);

#endif // SERIALIZATION_HPP
