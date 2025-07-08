#pragma once
#include "dynamicarray.h"
#include <stdexcept>
#include <atomic>

namespace type_id {

    // Use uint32_t for consistency, and define None_id outside struct
    inline constexpr uint32_t None_id = UINT32_MAX;

    // atomic counter of type uint32_t
    inline std::atomic<uint32_t> global_type_counter{ 0 };

    template<typename T>
    uint32_t get_typeid() {
        static const uint32_t id = global_type_counter.fetch_add(1, std::memory_order_relaxed);
        return id;
    }

    struct Id {
        uint32_t value;

        constexpr explicit Id(uint32_t val = None_id) : value(val) {}

        constexpr bool valid() const { return value != None_id; }
        constexpr bool operator==(const Id& other) const { return value == other.value; }
        constexpr bool operator!=(const Id& other) const { return value != other.value; }
        constexpr explicit operator bool() const { return valid(); }
    };

    inline constexpr Id None{ None_id };

    struct dense_type_system {
        uint32_t type_index = 0;
        Cont::array<Id> sparse_map;

        // Returns the number of types registered
        uint32_t size() const {
            return type_index;
        }

        template<typename T>
        Id get() {
            uint32_t id = get_typeid<T>();

          
            Id& dense_id = sparse_map[id];
            if (!dense_id.valid()) {
                dense_id = Id(type_index);
                ++type_index;
            }
            return dense_id;
        }

        template<typename T>
        bool contains() const {
            uint32_t typ_id = get_typeid<T>();
            if (typ_id >= sparse_map.length) {
                return false;
            }
            return sparse_map[typ_id].valid();
        }

        bool contains_id(uint32_t id) const {
            if (id >= sparse_map.length) {
                return false;
            }
            return sparse_map[id].valid();
        }
    };

}
