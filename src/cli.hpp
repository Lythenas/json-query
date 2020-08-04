#include <cstring>
#include <exception>
#include <optional>
#include <string>
#include <iostream>

namespace cli {

class CliException : public std::exception {
    public:
        CliException() {}
};

struct Arguments {
    bool help = false;
    bool only_parse = false;
    bool debug = false;
    std::string selector;
    std::optional<std::string> file;
};

void print_help(const char* name) {
    std::cerr
        << "Usage: " << name << " [--help] [--only-parse] [--debug] <selectors> [file]"
        << "\n\n"
        << "ARGS:" << std::endl
        << "\t<selectors>\tQuery selectors to apply\n"
        << "\t<file>\t\tJson file to use (if not given stdin will be used)\n"
        << "\n"
        << "OPTIONS:\n"
        << "\t--help\tPrints this help message and quits\n"
        << "\t--only-parse\tOnly parse the json and quits (useful for benchmarking)\n"
        << "\t--debug\tPrint debug information\n"
        << "\n"
        << "All diagnostics and errors are written to stderr and the json output "
        << "is written to stdout. So it is save to pipe the output to a file or command.\n";
}

/**
 * Parse arguments from argc and argv from main.
 *
 * The options have to come before the positional arguments.
 */
Arguments parse_arguments(int argc, char** argv) {
    Arguments args{};

    if (argc == 1) {
        args.help = true;
        return args;
    }

    bool error = false;
    int idx = 1;
    for (; argc >= idx + 1; ++idx) {
        std::string opt(argv[idx]);
        if (!opt.starts_with("--")) {
            break;
        }

        if (opt == "--help") {
            args.help = true;
            return args;
        } else if (opt == "--only-parse") {
            args.only_parse = true;
        } else if (opt == "--debug") {
            args.debug = true;
        } else {
            std::cerr << "Unrecognized option: \"" << opt << "\"\n\n";
            error = true;
        }
    }

    if (error) {
        print_help(argv[0]);
        throw CliException();
    }

    if (argc >= idx + 1) {
        args.selector = std::string(argv[idx]);
        idx++;
    } else {
        std::cerr << "<selectors> ist required\n\n";
        print_help(argv[0]);
        throw CliException();
    }

    if (argc >= idx + 1) {
        args.file = std::string(argv[idx]);
        // idx++;
    }

    return args;
}

} // namespace jq
