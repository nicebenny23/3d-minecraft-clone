#include "../../util/unique.h"
#include "../../util/dynamicarray.h"
#include "../../util/type_index.h"
#include "../../util/Option.h"
#include "../../util/pair.h"
#pragma once
namespace ecs {
	struct resource {
		virtual ~resource() {
		}
	};
	using resource_id = stn::typed_id<resource>;
	template<typename T>
	concept ResourceType = std::derived_from < T, resource>;
	struct Resources {
		stn::type_indexer<resource_id> indexer;
		stn::array<stn::box<resource>> resource_list;
		template<ResourceType T, typename ...Args>
		T& insert(Args&&... args) requires std::constructible_from<T, Args&&...> {
			stn::insertion<ecs::resource_id> insertion = indexer.insert<T>();
			if (insertion.is_not_new()) {
				//potential
				T* ptr = resource_list.unchecked_at(insertion.value.id).get_as_unchecked<T>();
				if (ptr) {
					return *ptr;
				}
			}
			resource_list.push(stn::box<T>(std::forward<Args>(args)...).upcast<resource>());
			return resource_list[insertion.value.id].ref_as_unchecked<T>();
		}
		template<ResourceType T>
		stn::Option<T&> get() {
			return indexer.get_opt<T>().and_then([&](ecs::resource_id id) {
				T* ptr = resource_list[id.id].get_as_unchecked<T>();
				if (ptr) {
					return stn::Option<T&>(*ptr);
				}
				return stn::Option<T&>(stn::None); });
		}
		template<ResourceType T>
		stn::Option<const T&> get() const {
			return indexer.get_opt<T>().and_then([&](resource_id id) {
				T* ptr = resource_list[id.id].get_as_unchecked<T>();
				if (ptr) {
					return stn::Option<const T&>(*ptr);
				}
				return stn::Option<T&>(stn::None); });
		}
		template<ResourceType T>
		T& ensure() {
			return insert<T>();
		}
		template<ResourceType T, typename Func>
		T& get_or_insert_with(Func&& func) {
			auto opt = get<T>();
			if (opt) return *opt;
			return insert<T>(func());
		}
		template<ResourceType T>
		bool has() const {
			return indexer.get_opt<T>().is_some_and([&](resource_id id) {return resource_list[id.id]; });
		}

		template<ResourceType T>
		void remove() {
			stn::Option<resource_id> insertion = indexer.get_opt<T>();
			if (insertion) {
				resource_list[insertion.unwrap().value].clear();
			}
		}
		void clear() {
			indexer.clear();
			resource_list.clear();
		}


	};
}