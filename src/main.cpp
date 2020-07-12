#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

#include "errors.hpp"
#include "json/json.hpp"
#include "selectors/parser.hpp"
#include "selectors/selectors.hpp"
#include "cli.hpp"

using json::Json;
using json::parse_json;
using selectors::FailedToParseSelectorException;
using selectors::parse_selectors;
using selectors::Selectors;

void print_arguments(const cli::Arguments& args) {
    std::cerr << "=== DEBUG ===" << std::endl;
    std::cerr << "Arguments {" << std::endl
              << "\thelp = " << args.help << "," << std::endl
              << "\tonly_parse = " << args.only_parse << "," << std::endl
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
    cli::Arguments args;
    try {
        args = cli::parse_arguments(argc, argv);

        if (args.help) {
            cli::print_help(argv[0]);
            return 0;
        }

        if (args.debug) {
            print_arguments(args);
        }

        std::string content = read_input(args.file);

        Json json = parse_json(content.begin(), content.end());

        Selectors selectors =
            parse_selectors(args.selector.begin(), args.selector.end());

        if (args.debug) {
            std::cerr << "json content:\n" << json << "\n";
            std::cerr << "selectors:\n" << selectors << "\n";
        }

        if (args.only_parse) {
            std::cerr << "Quitting after parse because of --only-parse flag.\n";
            return 0;
        }
        Json output = selectors.apply(json);

        std::cout << output;
    } catch (const InputFileException& e) {
        std::cerr << e.what() << std::endl;
    } catch (const FailedToParseSelectorException& e) {
        std::cerr << "Failed to parse selector: " << e.what() << std::endl;
    } catch (const selectors::SyntaxError& e) {
        e.pretty_print(std::cerr, args.selector);
    } catch (const cli::CliException&) {
        return 1;
    }

    return 0;
}
