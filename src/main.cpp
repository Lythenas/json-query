#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

#include "errors.hpp"
#include "json/json.hpp"
#include "selectors/selectors.hpp"

using json::Json;
using json::parse_json;
using selectors::FailedToParseSelectorException;
using selectors::parse_selectors;
using selectors::Selectors;

struct Arguments {
    bool help = false;
    std::string selector;
    std::optional<std::string> file;
};

void print_arguments(const Arguments& args) {
    std::cerr << "=== DEBUG ===" << std::endl;
    std::cerr << "Arguments {" << std::endl
              << "\thelp = " << args.help << "," << std::endl
              << "\tselector = \"" << args.selector << "\"," << std::endl
              << "\tfile = ";
    if (args.file) {
        std::cerr << "\"" << args.file.value() << "\"";
    } else {
        std::cerr << "none";
    }
    std::cerr << "," << std::endl << "}" << std::endl;
    std::cerr << "=== DEBUG END ===" << std::endl;
}

/**
 * Parses the arguemtns form argc and argv into and Arguments struct.
 */
Arguments parse_arguments(int argc, char* argv[]) {
    Arguments args;

    // no args or help flag display help
    if (argc == 1 || strcmp(argv[1], "-h") == 0 ||
        strcmp(argv[1], "--help") == 0) {
        args.help = true;
        return args;
    }

    if (argc >= 2) {
        args.selector = std::string(argv[1]);
    }

    if (argc >= 3) {
        args.file = std::string(argv[2]);
    }

    return args;
}

/**
 *  Reads complete input file (or stdin) to a string.
 *
 *  @param takes an optional file
 *  @throws InputFileException if there was an error reading the file
 */
std::string read_input(const std::optional<std::string>& file) {
    try {
        if (file) {
            std::ifstream ifs;
            ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ifs.open(file.value());
            return std::string(std::istreambuf_iterator<char>(ifs),
                               std::istreambuf_iterator<char>());
        } else {
            std::cin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            return std::string(std::istreambuf_iterator<char>(std::cin),
                               std::istreambuf_iterator<char>());
        }
    } catch (std::ifstream::failure&) {
        throw InputFileException();
    }
}

int main(int argc, char* argv[]) {
    const auto args = parse_arguments(argc, argv);

    if (args.help) {
        std::cerr
            << "Usage: " << argv[0] << " [-h|--help] <selectors> [file]"
            << std::endl
            << std::endl
            << "ARGS:" << std::endl
            << "\t<selectors>\tQuery selectors to apply" << std::endl
            << "\t<file>\t\tJson file to use (if not given stdin will be used)"
            << std::endl;
        return 0;
    }

    print_arguments(args);

    try {
        std::string content = read_input(args.file);

        Json json = parse_json(content.begin(), content.end());

        Selectors selectors =
            parse_selectors(args.selector.begin(), args.selector.end());

        std::cerr << "=== DEBUG ===" << std::endl;
        std::cerr << "content:" << std::endl << json << std::endl;
        std::cerr << "selectors:" << std::endl << selectors << std::endl;
        std::cerr << "=== DEBUG END ===" << std::endl;

        // Json output = selectors.run(json);

    } catch (const InputFileException& e) {
        std::cerr << e.what() << std::endl;
    } catch (const FailedToParseSelectorException& e) {
        std::cerr << "Failed to parse selector: " << e.what() << std::endl;
    }

    return 0;
}
