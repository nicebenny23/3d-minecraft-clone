#pragma once
#include "dynamicarray.h"
#include <stdexcept>
#include <atomic>
#include "Option.h"
#include <cstdint>
#include "pair.h"
#include "Id.h"
#include "Name.h"
#include <string>
#include "funtion.h"
#include <mutex>
namespace stn {

	template<typename T>
	struct type_indexer;

	template<typename T>
	struct type_map;
	// Global counter for assigning unique type IDs
	struct type_id_chart {
		stn::Option<stn::Stateless<bool(const void*, const void*)>> equals;
		stn::Option<stn::Stateless<std::size_t(const void*)>> hash;
		stn::Option < stn::Stateless<void(void*, const void*)>> copy_assign;
		stn::Option < stn::Stateless< void(void*, void*)>> move_assign;
		stn::Option < stn::Stateless<void(void*, const void*)>> copy_construct;
		stn::Option < stn::Stateless< void(void*, void*)>> move_construct;
		stn::Option<stn::Stateless<void(void*)>> default_construct;
		stn::Option<stn::Stateless<void(void*)>> destroy;
		stn::memory::layout layout;

		std::string name;

		template<typename T>
		static type_id_chart make() {
			type_id_chart c;
			c.name = std::string(typeid(T).name());
			c.layout = stn::memory::layout_of<T>;

			if constexpr (requires { std::hash<T>{}(std::declval<T>()); }) {
				c.hash = [](const void* a) -> std::size_t {
					return std::hash<T>{}(*static_cast<const T*>(a));
					};
			}
			if constexpr (std::equality_comparable<T>) {
				c.equals = [](const void* a, const void* b) -> bool {
					return *static_cast<const T*>(a) == *static_cast<const T*>(b);
					};
			}
			if constexpr (std::is_copy_constructible_v<T>) {
				c.copy_construct = [](void* dst, const void* src) {
					new (dst) T(*static_cast<const T*>(src));
					};
			}
			if constexpr (std::is_move_constructible_v<T>) {
				c.move_construct = [](void* dst, void* src) {
					new (dst) T(std::move(*static_cast<T*>(src)));
					};
			}
			if constexpr (std::is_copy_assignable_v<T>) {
				c.copy_assign = [](void* dst, const void* src) {
					*static_cast<T*>(dst) = *static_cast<const T*>(src);
					};
			}
			if constexpr (std::is_move_assignable_v<T>) {
				c.move_assign = +[](void* dst, void* src) {
					*static_cast<T*>(dst) = std::move(*static_cast<T*>(src));
					};
			}
			if constexpr (!std::is_trivially_destructible_v<T>) {
				c.destroy = [](void* obj) {
					static_cast<T*>(obj)->~T();
					};
			}
			if constexpr (std::is_default_constructible_v<T>) {
				c.default_construct = [](void* dst) {
					new (dst) T();
					};
			}
			return c;
		}
	};

	inline uint32_t global_type_counter{ 0 };
	inline stn::array<type_id_chart> global_type_charts;
	inline std::mutex type_registry_mutex;
	template<typename T>
	uint32_t register_type() {
			std::lock_guard lock(type_registry_mutex);
			uint32_t id = global_type_counter++;
			global_type_charts.push(type_id_chart::make< T >());
			return id;
	}

	struct type_id {
	
		bool operator==(type_id other) const {
			return id == other.id;
		}
		bool operator!=(type_id other) const {
			return id != other.id;
		}
		const type_id_chart& functions() const {
			return global_type_charts.unchecked_at(id);
		}

		stn::memory::layout layout() const {
			return functions().layout;
		}

		stn::Option<stn::Stateless<bool(const void*, const void*)>> equals() const {
			return functions().equals;
		}

		stn::Option<stn::Stateless<std::size_t(const void*)>> hash() const {
			return functions().hash;
		}

		stn::Option<stn::Stateless<void(void*, const void*)>> copy_construct() const {
			return functions().copy_construct;
		}

		stn::Option<stn::Stateless<void(void*)>> default_constructor() const {
			return functions().default_construct;
		}
		stn::Option<stn::Stateless<void(void*, void*)>> move_construct() const {
			return functions().move_construct;
		}

		stn::Option<stn::Stateless<void(void*, const void*)>> copy_assign() const {
			return functions().copy_assign;
		}

		stn::Option<stn::Stateless<void(void*, void*)>> move_assign() const {
			return functions().move_assign;
		}
		stn::Option<stn::Stateless<void(void*)>> destroy() const {
			return functions().destroy;
		}
		std::string_view name() const {
			return std::string_view(functions().name);
		}

	private:
		explicit type_id(uint32_t Id) : id(Id) {
		}

		template<typename T>
		friend struct type_map;
		template<typename T>
		friend struct type_indexer;
		template<typename T>
		friend type_id make_type_id();
		
		uint32_t id;
	};
	template<typename T>
	type_id make_type_id() {
		return type_id(typeIndex<T>());
	}

	// Retrieve a unique ID for type T
	template<typename T>
	uint32_t typeIndex() {
		static const uint32_t id = register_type<T>();
		return id;
	}


	template<typename id_type = stn::Id>
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
				stn::throw_logic_error("Id {} has not been created in the typelist", id.id);
			}
			return id_type(dense_id);
		}
		bool contains(type_id id) const {
			return id.id < sparse_map.length() && sparse_map[id.id] != invalid;
		}
		stn::Option<id_type> get_opt(type_id id) const {
			uint32_t dense_id = sparse_map.reach(id.id, invalid);
			if (dense_id == invalid) [[unlikely]] {
				return stn::None;
			}
			return id_type(dense_id);
		}

		template<typename T>
		stn::Option<id_type> get_opt() const {

			uint32_t id = typeIndex<T>();
			if (!sparse_map.contains_index(id)) {
				return stn::None;
			}
			uint32_t dense_id = sparse_map.unchecked_at(id);
			if (dense_id == invalid) [[unlikely]] {
				return stn::None;
			}
			return id_type(dense_id);
		}
		template<typename T>
		bool contains() const {
			uint32_t id = typeIndex<T>();
			return id < sparse_map.length() && sparse_map[id] != invalid;
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
	type_indexer() -> type_indexer<stn::Id>;

	template<typename T>
	struct type_map {
		//we can use an optional array here because the number of type ids is not high enough to impact us
		stn::array<stn::Option<T>> sparse_map;
		type_map() {
		}

		void set(stn::type_id id,const T& value) {
			sparse_map.reach(id.id) = value;
		}

		template<typename T>
		bool contains(stn::type_id id) const {
			return sparse_map.get_flat(id.id).has_value;
		}

		stn::Option<T> remove(stn::type_id id) {
			if (sparse_map.contains_index(id.id)) {
				sparse_map[id.id] = stn::None;
			}
		}
		
		stn::Option<T> at(stn::type_id id) const {
			return sparse_map.get_flat(id.id);
		}
		
		template<typename U>
		void set(const T& value) {
			sparse_map.reach(typeIndex<U>()) = value;
		}

		template<typename U>
		stn::Option<T> remove() {
			size_t index = typeIndex<U>();
			if (sparse_map.contains_index(index)) {
				sparse_map[index] = stn::None;
			}
		}

		template<typename U>
		bool contains() const {
			return sparse_map.get_flat(typeIndex<U>()).has_value;
		}

		template<typename U>
		stn::Option<T> at() const {
			return sparse_map.get_flat(typeIndex<U>());
		}
		
	};
}
