#pragma once
#include "dynamicarray.h"
#include <stdexcept>
#include <atomic>
#include "Option.h"
#include <cstdint>
#include "pair.h"
#include "Id.h"
namespace type_id {


    // Global counter for assigning unique type IDs
    inline std::atomic<uint32_t> global_type_counter{ 0 };

    // Retrieve a unique ID for type T
    template<typename T>
    uint32_t typeIndex() {
        static const uint32_t id = global_type_counter.fetch_add(1, std::memory_order_relaxed);
        return id;
    }

  
    struct type_indexer {
        uint32_t type_index = 0;
        stn::array<Ids::Id> sparse_map;

        uint32_t size() const {
            return type_index;
        }

        template<typename T>
        Ids::Id get() const{
            uint32_t id = typeIndex<T>();
            Ids::Id dense_id = sparse_map[id];
            if (!dense_id.valid()) {
                throw std::logic_error(std::string("Id of" + std::string(typeid(T).name()) + "has not been created"));
            }
            return dense_id;
        }

        template<typename T>
        Opt::Option<Ids::Id> get_opt() {
            Ids::Id dense_id = sparse_map.reach(typeIndex<T>());
            if (!dense_id.valid()) [[unlikely]] {
                return Opt::None; 
            }
            return Opt::Option<Ids::Id>(dense_id);
        }
        template<typename T>
        util::pair<Ids::Id, bool> insert() {
            uint32_t id = typeIndex<T>();
            Ids::Id& dense_id = sparse_map.reach(id);
            bool is_new = false;
            if (!dense_id.valid()) {
                dense_id = Ids::Id(type_index++);
                is_new = true;
            }
            return { dense_id, is_new };
        }
        template<typename T>
        bool contains() const {
            uint32_t id = typeIndex<T>();
            return id < sparse_map.length && sparse_map[id].valid();
        }

        template <typename... Types>
        stn::array<Ids::Id> get_type_ids() {
            return { get<Types>()... };
        }


         template <typename... Types>
           bool contains_all() const {
               return (contains<Types>() && ...);
           }
    };

}
