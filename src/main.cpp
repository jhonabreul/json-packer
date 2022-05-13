#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#include "json_packer.hpp"

struct Arguments
{
    std::string input_filename;
    std::string output_filename;
    bool unpack = false;
};

auto parseArguments(int argc, char *argv[])
{
    Arguments args;
    int opt;

    while ((opt = getopt(argc, argv, "i:o:uh")) != -1) {
        switch (opt) {
        case 'i':
            args.input_filename = optarg;
            break;

        case 'o':
            args.output_filename = optarg;
            break;

        case 'u':
            args.unpack = true;
            break;

        case 'h':
        case '?':
        default:
            std::cerr << "Usage: "
                      << argv[0]
                      << " [-i input_filename] [-o output_filename]\n\n"
                      << "stdin and stdout will be used in the absence of "
                      << "input and output filenames.\n";
            exit(EXIT_FAILURE);
        }
    }


    if (args.output_filename.empty()) {
        std::cerr << "The output filename is mandatory.\n";
        exit(EXIT_FAILURE);
    }

    return args;
}

template<typename T>
T openStream(const std::string & filename, bool binary = false)
{
    if (filename.empty()) {
        return T();
    }

    T stream;

    if (binary) {
        stream.open(filename, std::ios_base::binary);
    } else {
        stream.open(filename);
    }

    if (!stream) {
        std::cerr << "Input file " << filename
                  << "could not be opened\n";
        exit(EXIT_FAILURE);
    }

    return stream;
}

int main(int argc, char *argv[])
{
    Arguments args = parseArguments(argc, argv);
    std::ifstream in;
    std::ofstream out;

    if (args.unpack) {
        in = openStream<std::ifstream>(args.input_filename, true);
        out = openStream<std::ofstream>(args.output_filename);
        JsonPacker::unpack(in, out);
    } else {
        in = openStream<std::ifstream>(args.input_filename);
        out = openStream<std::ofstream>(args.output_filename, true);
        JsonPacker::pack(in, out);
    }
}
