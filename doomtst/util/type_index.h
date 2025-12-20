#pragma once
#include "dynamicarray.h"
#include <stdexcept>
#include <atomic>
#include "Option.h"
#include <cstdint>
#include "pair.h"
#include "Id.h"
namespace stn {


    // Global counter for assigning unique type IDs
    inline std::atomic<uint32_t> global_type_counter{ 0 };

    // Retrieve a unique ID for type T
    template<typename T>
    uint32_t typeIndex() {
        static const uint32_t id = global_type_counter.fetch_add(1, std::memory_order_relaxed);
        return id;
    }
    struct type_id {
        template<typename T>
        static type_id make_type_id() {
            return type_id(typeIndex<T>());
        }
        bool operator==(type_id other) const {
            return id == other.id;
        }
        bool operator!=(type_id other) const {
            return id != other.id;
        }
    private:
        type_id(uint32_t Id) :id(Id) {};
        uint32_t id;
    };
    template<typename id_type=stn::Id>
    struct type_indexer {
        uint32_t next_index = 0;
        stn::array<id_type> sparse_map;
        type_indexer() {
            sparse_map = stn::array<id_type>();
        }
        uint32_t size() const {
            return next_index;
        }
        template<typename T>
        stn::insertion<id_type> insert() {

            id_type& dense_id = sparse_map.reach(typeIndex<T>(), id_type(-1));
            if (dense_id == id_type(-1)) {
                dense_id = id_type(next_index++);
                return stn::insertion(dense_id, true);
            }
            return  stn::insertion(dense_id, false);

        }
		template<typename T>
		id_type get_unchecked() const {
				return sparse_map.unchecked_at(typeIndex<T>());
		}
		template<typename T>
        id_type get() const {

            id_type dense_id = sparse_map[typeIndex<T>()];
            if (dense_id == id_type(-1)) {
                throw std::logic_error(std::string("Id of" + std::string(typeid(T).name()) + "has not been created"));
            }
            return dense_id;
        }
        template<typename T>
        stn::Option<id_type> get_opt() {
            id_type dense_id = sparse_map.reach(typeIndex<T>(),id_type(-1));
            if (dense_id== id_type(-1)) [[unlikely]] {
                return stn::None;
            }
            return dense_id;
        }
        template<typename T>
        bool contains() const {
            uint32_t id = typeIndex<T>();
            return id < sparse_map.length() && sparse_map[id]!= id_type(-1);
        }

        template <typename... Types>
        stn::array<stn::insertion<id_type>> insert_ids() {
            return stn::array({ insert<Types>()... });
        }
        template <typename... Types>
        stn::array<id_type> get_ids() {
            return stn::array({ get<Types>()... });
        }
        template <typename... Types>
        stn::array<stn::Option<id_type>> get_opt_ids() {
            return stn::array({ get_opt<Types>()... });
        }
        template <typename... Types>
        bool contains_ids() const {
            return (contains<Types>() && ...);
        }
        void clear() {
            sparse_map.clear();
            next_index = 0;
        }
    };
    type_indexer()->type_indexer<stn::Id>;

}
