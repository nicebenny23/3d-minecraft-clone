#pragma once
#include "dynamicarray.h"
#include <stdexcept>
#include <atomic>
#include "Option.h"
#include <cstdint>
#include "pair.h"
namespace type_id {

    // Unique ID for an invalid type
    inline constexpr uint32_t None_id = UINT32_MAX;

    // Global counter for assigning unique type IDs
    inline std::atomic<uint32_t> global_type_counter{ 0 };

    // Retrieve a unique ID for type T
    template<typename T>
    uint32_t typeIndex() {
        static const uint32_t id = global_type_counter.fetch_add(1, std::memory_order_relaxed);
        return id;
    }

    struct Id {
        uint32_t value;

        constexpr explicit Id(uint32_t val = None_id) : value(val) {}

        constexpr bool inline valid() const { return value != None_id; }
        constexpr bool operator==(const Id& other) const { return value == other.value; }
        constexpr bool operator!=(const Id& other) const { return value != other.value; }
        constexpr explicit operator bool() const { return valid(); }
    };

    inline constexpr Id None{ None_id };

    struct type_indexer {
        uint32_t type_index = 0;
        Cont::array<Id> sparse_map;

        uint32_t size() const {
            return type_index;
        }

        template<typename T>
        Id get() const{
            uint32_t id = typeIndex<T>();
            Id dense_id = sparse_map[id];
            if (!dense_id.valid()) {
                throw std::logic_error(std::string("Id of" + std::string(typeid(T).name()) + "has not been created"));
            }
            return dense_id;
        }

        template<typename T>
        Opt::Option<Id> get_opt() {
            Id dense_id = sparse_map.reach(typeIndex<T>());
            if (!dense_id.valid()) [[unlikely]] {
                return Opt::None; 
            }
            return Opt::Option<Id>(dense_id);
        }
        template<typename T>
        util::Pair<Id, bool> insert() {
            uint32_t id = typeIndex<T>();
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
            uint32_t id = typeIndex<T>();
            return id < sparse_map.length && sparse_map.contains(id);
        }

        template <typename... Types>
        Cont::array<Id> get_type_ids() {
            return { get<Types>()... };
        }
    };

}
