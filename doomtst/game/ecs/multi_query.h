#pragma once
#include "../../util/thread_split.h"
#include "query.h"
#include "../../util/thread_pool.h"
#pragma once
namespace ecs {
	template<typename... Components>
	using query_func = std::function<void(std::tuple<Components&...>)>;
	template<ComponentType... Components>
	void multi_iter(View<Components...>& view,archetype_id id,archetype_index start, archetype_index end_index, query_func<Components&...> query) {
		Archetype& arch = view.world().archetypes.archetype_at(id);
		if (end_index< start)
		{
			stn::throw_invalid_argument("end index {} may not be greater than start index", end_index , start);
		}
		archetype_index index=start;
		while (index != end_index) {
			query(view.world().get_tuple_unchecked<Components...>(arch.elems.unchecked_at(index.index), view.view_indices()));
			index++;
		}
	}
	

	template<ComponentType... Components>
	void multi_query(View<Components...>& view, query_func<Components&...> quer, size_t threads_wanted, size_t count) {
		if (count==0)
		{
			stn::throw_logic_error("count may not be 0");
		}
		if (threads_wanted== 0)
		{
			stn::throw_logic_error("total threads may not be 0");
		}
		thread::thread_pool pool(threads_wanted);

		for (size_t i = 0; i < view.total_archetypes(); i++)
		{
			Archetype& arch = view.archetype_at(i);
			stn::array<archetype_index> running_locations;
			//take number of tasks excluding the last one
			size_t nonterminal_tasks= std::floor(arch.count()/count);
			archetype_index current = archetype_index(0);
			if (arch.elems.empty()) {
				continue;
			}
				running_locations.push(current);
				for (size_t i = 0; i < nonterminal_tasks; i++) {
					current += stn::typed_count<ecs::ArchetypeIndexTag>(count);
					running_locations.push(current);
				}
				running_locations.push(arch.last_index());
			for (size_t i = 0; i < running_locations.length()-2; i++)
			{  
				archetype_index first = running_locations[i];
				archetype_index last= running_locations[i+1];
				archetype_id id = arch.id();
				pool.push([&view, id, first, last, quer]() {
					multi_iter(view, id, first, last, quer);
					});
			}
		}
	}
	template<ComponentType... Components>
	void multi_query(View<Components...>& view, query_func<Components&...> quer, size_t threads_wanted) {
		size_t total_length = 0;
		for (auto& arch : view.view_archetypes())
		{
			total_length += view.world().archetypes.archetype_at(arch).count();
		}
		multi_query(view, quer, threads_wanted, 1 + total_length / threads_wanted);

	}
}