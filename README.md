# JSON Packer

## Building

```sh
$ mkdir build && cd build
$ cmake -DJP_BUILD_TESTS=ON ..
$ make
$ cd ..
```

## Rnning tests

```sh
$ ./build/bin/json_packer_tests
```

## Rnning command line program

### Help

```sh
$ ./build/bin/json_packer -h
```

### Pack

```sh
$ ./build/bin/json_packer -i input.txt -o output.bin
```

### Unpack

```sh
$ ./build/bin/json_packer -u -i input.bin -o output.txt
```

## Encoding format

### Tag

The tag is the identifier assigned to each type that can be encoded. For simple JSON values, 3 bits are enough.

### Length

There are two possibilities for the endcoding format, based of the amount of bytes required to represent the length of the value. The bit 5 of the first byte will encoded the format type, 0 for short and 1 for long format.

#### Short format

If bit 5 of the first byte is 0, the length is in short format, that is, it is encoded in the 4 remaining bits of the first byte. In this case, the value is encoded starting from byte 2.

<table>
    <thead>
        <tr>
            <th colspan="8">Byte 1</th>
            <th colspan="8">Bytes 2 ... n</th>
        </tr>
        <tr>
            <th>8</th>
            <th>7</th>
            <th>6</th>
            <th>5</th>
            <th>4</th>
            <th>3</th>
            <th>2</th>
            <th>1</th>
            <th>8</th>
            <th>7</th>
            <th>6</th>
            <th>5</th>
            <th>4</th>
            <th>3</th>
            <th>2</th>
            <th>1</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td colspan="3">Tag</td>
            <td>0</td>
            <td colspan="4">Length</td>
            <td colspan="8">Value</td>
        </tr>
    </tbody>
</table>

#### Long format

If bit 5 of the first byte is 1, the length is in long format, that is, the 4 remaining bits of the first byte encode the length of the length (the amounts of bytes starting from the next one that encode the value length). In this case, the value is encoded starting from byte 2+l.

<table>
    <thead>
        <tr>
            <th colspan="8">Byte 1</th>
            <th colspan="8">Bytes 2 ... l+1</th>
            <th colspan="8">Bytes l+2 ... n</th>
        </tr>
        <tr>
            <th>8</th>
            <th>7</th>
            <th>6</th>
            <th>5</th>
            <th>4</th>
            <th>3</th>
            <th>2</th>
            <th>1</th>
            <th>8</th>
            <th>7</th>
            <th>6</th>
            <th>5</th>
            <th>4</th>
            <th>3</th>
            <th>2</th>
            <th>1</th>
            <th>8</th>
            <th>7</th>
            <th>6</th>
            <th>5</th>
            <th>4</th>
            <th>3</th>
            <th>2</th>
            <th>1</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td colspan="3">Tag</td>
            <td>1</td>
            <td colspan="4">Length of length (l)</td>
            <td colspan="8">Length</td>
            <td colspan="8">Value</td>
        </tr>
    </tbody>
</table>

## Zero-length values

There are cases in which the value is implicitly stored in the length, as is the case of boolean `false`. This value would be serialized with length 0 and no value bytes, which would be interpreted as the boolan `false`. The same applies for integer 0, empty strings and the null value.

## High-level design

![Class diagram](/class_diagram.png)

Polymorphism was used to model the serialization/deserialization algorithms depending on the type of data to be serialized. `JsonTLVObject` models a TLV serializable object and includes the abstract methods `serialize()` and `deserialize()`.

To hold values of different types, I implemented a basic version of Type Erasure, with the template class `JsonTLVValueHolder<T>` deriving `JsonTLVObject` and implementing the required features to hold a value of type `T`. This template does not implement the `serialize()` and `deserialize()` methods, so this template instanciates abstract classes.

Finally, each type can derive this class and implement the methods to serialize and deserializes values of said type. For example, a class to hold and serialize/deserialize boolean values would inherit the class `JsonTLVValueHolder<bool>`.

## Room for improvement

Due to timing issues, there is a lot of room for improvement in this project, including:

- **Negative unit tests**: there are still a lot of tests to be implemented, especially negative tests, to ensure correct behavior on bad input, like malformed files or malformed/incorrect binary inputs for decompression.

- **Design improvements**: this is only a first design for this packer, in which I tried to include features that could showcase my C++ knowledge. I know there are ways to simplify the design.

- **Floating point numbers serialization**: floating point numbers serialization is not a trivial task if you consider portability, which is why I decided to use the serialization features included with the JSON library I used. In this case, floating point numbers were serialized using nlohmann JSON's CBOR implementation.

- **In-code documentation**: write in-code comments and use something like Doxygen to generate proper documentation. This would include documenting classes to describe their purpose and functions/methods to describe parameters, functionality, return value and/or exceptions or errors.
