#pragma once
#include <cstdint>
#include <limits>

namespace comp {
    inline constexpr uint32_t None_id = std::numeric_limits<uint32_t>::max();

    struct Id {
        uint32_t value;

        constexpr explicit Id(const uint32_t& val) : value(val) {}
        constexpr Id() : value(None_id) {}

        constexpr bool valid() const { return value != None_id; }
        constexpr bool operator==(const Id& other) const { return value == other.value; }
        constexpr bool operator!=(const Id& other) const { return value != other.value; }
        constexpr explicit operator bool() const { return valid(); }
    };

    inline constexpr Id None{ None_id };
}
