#ifndef SERIALIZATION_HPP
#define SERIALIZATION_HPP

#include <vector>
#include <cstdint>
#include <type_traits>

#define INTEGRAL_TYPE_TEMPLATE                                                 \
    template<                                                                  \
        class T,                                                               \
        typename = typename std::enable_if_t<std::is_arithmetic<T>::value, T>  \
    >

INTEGRAL_TYPE_TEMPLATE
std::vector<uint8_t> serializeNumericValue(T value)
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
static T deserializeNumericValue(std::vector<uint8_t>::const_iterator start,
                                 std::vector<uint8_t>::const_iterator end)
{
    constexpr auto type_size = sizeof(T);
    // TODO: This should better be an exception
    assert(end - start <= type_size);

    T value = 0;
    for (int i = 0; start < end; i++) {
        value |= (T) *start++ << (i * 8);
    }

    return value;
}

INTEGRAL_TYPE_TEMPLATE
static T deserializeNumericValue(const std::vector<uint8_t> & bytes)
{
    return deserializeNumericValue<T>(bytes.begin(), bytes.end());
}

#endif // SERIALIZATION_HPP
