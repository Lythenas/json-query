#include <exception>

class AppException : public std::exception {};

class InputFileException : public AppException {
  public:
    const char *what() const noexcept override {
        return "error reading input file";
    }
};
