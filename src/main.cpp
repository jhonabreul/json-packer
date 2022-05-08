#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#include "json_packer.hpp"

struct Arguments
{
    std::string input_filename;
    std::string output_filename;
};

auto parseArguments(int argc, char *argv[])
{
    Arguments args;
    int opt;

    while ((opt = getopt(argc, argv, "i:o:h")) != -1) {
        switch (opt) {
        case 'i':
            args.input_filename = optarg;
            break;

        case 'o':
            args.output_filename = optarg;
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

    return args;
}

template<typename T>
T openStream(const std::string & filename)
{
    if (filename.empty()) {
        return T();
    }

    T stream(filename);

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

    auto in = openStream<std::ifstream>(args.input_filename);
    auto out = openStream<std::ofstream>(args.output_filename);

    JsonPacker::pack(in.is_open() ? in : std::cin, out.is_open() ? out : std::cout);
}
