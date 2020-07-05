#include "types.hpp"

// Here are the things that can't be defined in types.hpp because of incomplete
// type errors. Grrrrr

namespace json {

    // class JsonString
    std::ostream& operator<<(std::ostream& o, const JsonString& self) {
        return o << "\"" << self.str << "\"";
    }

    // class JsonObject
    bool JsonObject::operator==(const JsonObject& other) const {
        return this->members == other.members;
    }
    std::ostream& operator<<(std::ostream& o, const JsonObject& self) {
        o << "{";

        const char* sep = "";
        for (const auto& i : self.members) {
            o << sep << "\"" << i.first << "\": " << i.second;
            sep = ",";
        }

        return o << "}";
    }

    // class Array
    std::ostream& operator<<(std::ostream& o, const JsonArray& self) {
        o << "[";

        const char* sep = "";
        for (const auto& i : self.items) {
            o << sep << i;
            sep = ",";
        }

        return o << "]";
    }

} // namespace json
