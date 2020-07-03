#include "types.hpp"

std::ostream& operator<<(std::ostream& o, const JsonObject& self) {
    o << "{";

    const char* sep = "";
    for (const auto& i : self.members) {
        o << sep << "\"" << i.first << "\": " << i.second;
        sep = ",";
    }

    return o << "}";
}

std::ostream& operator<<(std::ostream& o, const JsonArray& self) {
    o << "[";

    const char* sep = "";
    for (const auto& i : self.items) {
        o << sep << i;
        sep = ",";
    }

    return o << "]";
}
