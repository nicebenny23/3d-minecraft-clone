#pragma once
#include "dynamicarray.h"
#include <stdexcept>
#include <atomic>
#include "Option.h"
#include <cstdint>
#include "pair.h"
#include "Id.h"
#include "Name.h"
namespace stn {

	template<typename T>
	struct type_indexer;
    // Global counter for assigning unique type IDs
	struct functions_for {
		stn::Option<std::function<bool(const void*, const void*)>> equals;
		stn::Option<std::function<std::size_t(const void*)>> hash;
	};

    inline std::atomic<uint32_t> global_type_counter{ 0 };
	inline stn::array<functions_for> g_type_functions;
	template<typename T>
	uint32_t register_type() {
		uint32_t new_id = global_type_counter.fetch_add(1, std::memory_order_relaxed);

		//need to make threadsafe
		functions_for funcs;
		if constexpr (!std::is_empty_v<std::hash<T>>) {
			funcs.hash = [](const void* a) -> std::size_t {
				return std::hash<T>{}(*static_cast<const T*>(a));
				};
		}
		if constexpr (std::equality_comparable<T>) {

			funcs.equals = [](const void* a, const void* b) -> bool {
				return *static_cast<const T*>(a) == *static_cast<const T*>(b);
				};
		}
		g_type_functions.push(funcs);
		return new_id;
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
		friend stn::Option<std::function<bool(const void*, const void*)>> equal_for(type_id id);
		friend stn::Option<std::function<std::size_t(const void*)>> hash_for(type_id id);
		explicit type_id(uint32_t Id) : id(Id) {
		}

		template<typename T>
		friend struct type_indexer;
		uint32_t id;
	};

	// Get the equality function for a type_id
	inline stn::Option<std::function<bool(const void*, const void*)>> equal_for(type_id id) {
		return g_type_functions[id.id].equals;
	}

	// Get the hash function for a type_id
	inline stn::Option<std::function<std::size_t(const void*)>> hash_for(type_id id) {
		return g_type_functions[id.id].hash;
	}

    // Retrieve a unique ID for type T
    template<typename T>
    uint32_t typeIndex() {
        static const uint32_t id = register_type<T>();
        return id;
    }
	

    template<typename id_type=stn::Id>
    struct type_indexer {
        uint32_t next_index = 0;
		static constexpr uint32_t invalid = std::numeric_limits< uint32_t>::max();
        stn::array<uint32_t> sparse_map;
        type_indexer() {
            sparse_map = stn::array<uint32_t>();
        }
        uint32_t size() const {
            return next_index;
        }

        template<typename T>
        stn::insertion<id_type> insert() {

			uint32_t& dense_id = sparse_map.reach(typeIndex<T>(), invalid);
            if (dense_id == invalid) {
                dense_id = next_index++;
                return stn::insertion(id_type(dense_id), true);
            }
            return  stn::insertion(id_type(dense_id), false);

        }
		template<typename T>
		id_type get_unchecked() const {
				return id_type(sparse_map.unchecked_at(typeIndex<T>()));
		}
		template<typename T>
        id_type get() const {

			uint32_t dense_id = sparse_map[typeIndex<T>()];
            if (dense_id == invalid) {
                throw std::logic_error(std::string("Id of" + std::string(typeid(T).name()) + "has not been created"));
            }
            return id_type(dense_id);
        }

		id_type get(type_id id) const {
			uint32_t dense_id = sparse_map[id.id];
			if (dense_id == invalid) {
				stn::throw_logic_error("Id {} has not been created in the typelist",id.id);
			}
			return id_type(dense_id);
		}
		bool contains(type_id id) const {
			return id.id < sparse_map.length() && sparse_map[id.id] != invalid;
		}
		stn::Option<id_type> get_opt(type_id id) const{
			uint32_t dense_id = sparse_map.reach(id.id, invalid);
			if (dense_id == invalid) [[unlikely]] {
				return stn::None;
			}
			return id_type(dense_id);
		}

		template<typename T>
        stn::Option<id_type> get_opt() const{

			uint32_t id = typeIndex<T>();
			if (!sparse_map.contains_index(id)) {
				return stn::None;
			}
			uint32_t dense_id = sparse_map.unchecked_at(id);
            if (dense_id== invalid) [[unlikely]] {
                return stn::None;
            }
            return id_type(dense_id);
        }
        template<typename T>
        bool contains() const {
            uint32_t id = typeIndex<T>();
            return id < sparse_map.length() && sparse_map[id]!= invalid;
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
		~type_indexer() {
			clear();
		}
    };
    type_indexer()->type_indexer<stn::Id>;

}
