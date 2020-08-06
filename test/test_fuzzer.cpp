#include <bits/stdint-uintn.h>
#include <cstddef>
#include <string>

#include "selectors/selectors.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    try {
        const std::string s(data, data + size);
        selectors::parse_selectors(s);
    } catch (selectors::FailedToParseSelectorException& e) {
    } catch (selectors::SyntaxError& e) {
    }

    return 0;
}
