#pragma once
#include "../../util/thread_split.h"

#include "../../util/thread_pool.h"
#include "component.h"
#include <type_traits>
#include "archtype.h"
#include "entities.h"
#include "../../util/exception.h"
#include "../../util/index.h"
#include "query.h"
#pragma once
namespace ecs {
	template<typename Func,typename... Components>
	concept query_function = std::is_invocable_v<Func,std::tuple<Components&>...>&&ComponentType<Components...>;

	template<typename Func, ComponentType... Components> requires query_function<Func,Components...>
	void multi_iter(View<Components...>& view, archetype_id id, archetype_index start_index, archetype_index end_index, Func query) {
		Archetype& arch = view.world().archetypes.archetype_at(id);
		if (end_index < start_index) {
			stn::throw_invalid_argument("end index {} may not be greater than start index", end_index, start_index);
		}
		archetype_index index = start_index;
		stn::span<const component_id> indices= view.view_indices();
		Ecs& world=view.world();
		while (index != end_index) {
			query(world.get_tuple_unchecked<Components...>(arch.elems.unchecked_at(index.index), indices));
			index++;
		}
	}


	template<typename Func, ComponentType... Components> requires query_function<Func, Components...>
	void multi_query(View<Components...>& view, Func quer, size_t threads_wanted, std::uint32_t count) {
		if (count == 0) {
			stn::throw_logic_error("count may not be 0");
		}
		if (threads_wanted == 0) {
			stn::throw_logic_error("total threads may not be 0");
		}
		thread::thread_pool pool(threads_wanted);

		for (size_t i = 0; i < view.total_archetypes(); i++) {
			Archetype& arch = view.archetype_at(i);
			archetype_id id = arch.id();
			archetype_index current = archetype_index(0);
			archetype_index final = archetype_index(arch.count());
			while (current != final) {
				archetype_index next = std::min(final, current + archetype_index::count_type(count));
				pool.push([view_ptr = &view, id, current, next, quer]() {
					multi_iter(*view_ptr, id, current, next, quer);
					});
				current = next;
			}
		}
	}
	template<typename Func, ComponentType... Components> requires query_function<Func, Components...>
	void multi_query(View<Components...>& view, size_t threads_wanted, Func quer) {
		size_t total_length = 0;
		for (auto& arch : view.view_archetypes()) {
			total_length += view.world().archetypes.archetype_at(arch).count();
		}
		multi_query(view, quer, threads_wanted, 1 + total_length / threads_wanted);

	}
}