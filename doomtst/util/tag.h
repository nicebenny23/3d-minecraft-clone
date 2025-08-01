#include <string>
#include <stdexcept>
#include <algorithm>
namespace tag {
    class Tag {
        std::string value;

        static bool is_valid(const std::string& str) {
            return !str.empty() && std::all_of(str.begin(), str.end(), [](char c) {
                return std::islower(c) || std::isdigit(c) || c == '_';
                });
        }

    public:
        Tag() = default;

        explicit Tag(const std::string& str) {
            if (!is_valid(str)) {
                throw std::invalid_argument("Tag must be lowercase and contain only a-z, 0-9, or underscores.");
            }
            value = str;
        }
        explicit Tag(const char* str) {
            if (!is_valid(std::string(str))) {
                throw std::invalid_argument("Tag must be lowercase and contain only a-z, 0-9, or underscores.");
            }
            value = str;
        }
        const std::string& str() const { return value; }

        bool operator==(const Tag& other) const { return value == other.value; }
        bool operator!=(const Tag& other) const { return value != other.value; }
        bool operator==(const char*& other) const { return value == other; }
        bool operator!=(const char*& other) const { return value != other; }
        bool operator==(const std::string& other) const { return value == other; }
        bool operator!=(const std::string& other) const { return value != other; }
        bool empty() const { return value.empty(); }

        struct Hasher {
            std::size_t operator()(const Tag& t) const {
                return std::hash<std::string>{}(t.value);
            }
        };
    };
}
