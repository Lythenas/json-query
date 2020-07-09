#include "types.hpp"

// Here are the things that can't be defined in types.hpp because of incomplete
// type errors. Grrrrr

namespace json {

// class JsonString
std::ostream& operator<<(std::ostream& o, const JsonString& self) {
    return o << "\"" << self.str << "\"";
}

// class JsonObject
const JsonNode& JsonObject::find(const std::string& key) const {
    auto predicate = [&key](const auto& val) { return val.first == key; };
    auto found = std::find_if(members.cbegin(), members.cend(), predicate);
    if (found == members.cend()) {
        throw std::out_of_range("json object does not contain key");
    }
    return (*found).second;
}

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

}  // namespace json
