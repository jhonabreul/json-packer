#ifndef BINARY_OUTPUT_STREAM_HPP
#define BINARY_OUTPUT_STREAM_HPP

#include <cstdint>
#include <vector>
#include <cstddef>
#include <utility>
#include <cassert>
#include <algorithm>
#include <fstream>

using Byte = uint8_t;
using ByteArray = std::vector<Byte>;
using ByteArrayIterator = ByteArray::const_iterator;

class BinaryOutputStream
{
    std::ostream & stream;

public:

    BinaryOutputStream(std::ostream & _stream): stream(_stream) {}

    void write(const ByteArray & bytes)
    {
        this->stream.write(reinterpret_cast<const char *>(bytes.data()),
                           bytes.size() * sizeof(Byte));
    }
};

class BinaryInputStream
{
    std::istream & stream;

public:

    BinaryInputStream(std::istream & _stream): stream(_stream) {}

    ByteArray read(size_t n_bytes)
    {
        ByteArray bytes(n_bytes);
        if (!this->stream.read(reinterpret_cast<char *>(bytes.data()),
                               n_bytes * sizeof(Byte))) {
            // TODO: Throw exception
            assert(("Buffer does not have enough bytes to read", false));
        }

        return bytes;
    }
};

#endif // BINARY_OUTPUT_STREAM_HPP
