#pragma once
#include "dynamicarray.h"
#include <stdexcept>
#include <atomic>
#include <cstdint>
#include "pair.h"
namespace type_id {

    // Unique ID for an invalid type
    inline constexpr uint32_t None_id = UINT32_MAX;

    // Global counter for assigning unique type IDs
    inline std::atomic<uint32_t> global_type_counter{ 0 };

    // Retrieve a unique ID for type T
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

        uint32_t size() const {
            return type_index;
        }

        template<typename T>
        Id get() {
            uint32_t id = get_typeid<T>();
            Id& dense_id = sparse_map.reach(id);
            if (!dense_id.valid()) {
                dense_id = Id(type_index++);
            }
            return dense_id;
        }
        template<typename T>
        util::Pair<Id, bool> insert() {
            uint32_t id = get_typeid<T>();
            Id& dense_id = sparse_map.reach(id);
            bool is_new = false;
            if (!dense_id.valid()) {
                dense_id = Id(type_index++);
                is_new = true;
            }
            return { dense_id, is_new };
        }
        template<typename T>
        bool contains() const {
            uint32_t id = get_typeid<T>();
            return id < sparse_map.length && sparse_map[id].valid();
        }

        template <typename... Types>
        Cont::array<Id> get_type_ids() {
            return { get<Types>()... };
        }
    };

}
