#pragma once

#include <boost/spirit/home/support/info.hpp>
#include <exception>
#include <string>

namespace errors {

class AppException : public std::exception {};

class InputFileException : public AppException {
public:
    const char* what() const noexcept override {
        return "error reading input file";
    }
};

class ParseError : public std::exception {
    protected:
        std::string what_;
};
class SyntaxError : public ParseError {
public:
    std::size_t error_pos;
    std::string expected;


    template <typename Iterator>
    SyntaxError(Iterator first, Iterator last, Iterator error_pos,
                const boost::spirit::info& what)
        : error_pos(std::distance(first, error_pos)) {
        std::stringstream ss;
        ss << what;
        expected = ss.str();

        what_ = "Expected " + expected + " but got \"" +
                std::string(error_pos, last) + "\"";
    }

    // to make this a proper std::exception
    // but not used by me (except maybe in tests)
    virtual const char* what() const noexcept override { return what_.c_str(); }

    template <typename Out>
    void pretty_print(Out& o, const std::string& input) const {
        assert(input.size() > error_pos);

        o << "Error in selector:\n"
          << "\033[32m" << input.substr(0, error_pos) << "\033[31m";

        if (error_pos == input.size()) {
            o << "\033[7m"
              << " ";
        } else {
            o << input.substr(error_pos);
        }

        o << "\033[0m\n"
          << std::string(error_pos, ' ') << "^ expected \033[32m" << expected
          << "\033[0m\n";
    }
};

} // namespace errors
