#pragma once
#include "../../util/thread_split.h"
#include "query.h"
#include "../../util/thread_pool.h"
#pragma once
namespace ecs {
	template<typename... Components>
	using query_func = std::function<void(std::tuple<Components*...>)>;
	template<typename... Components>
	void multi_iter(Ecs& context,archetype_id id,archetype_index start, archetype_location end_index, query_func<Components*...> query) {
		Archetype& arch = context.archetypes[id];
		if (end_index< start)
		{
			stn::throw_invalid_argument("end index {} may not be greater than start index", end_index , start);
		}
		archetype_index index=start;
		while (index != end_index) {
			query(context.get_tuple_unchecked<Components...>(arch.elems.unchecked_at(index)));
			index++;
		}
	}
	

	template<typename... Components>
	void multi_query(View<Components...>& view, query_func<Components*...> quer, size_t threads_wanted, size_t count) {
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
			running_locations.push(current);
			for (size_t i = 0; i < nonterminal_tasks; i++)
			{
				current += count;
				running_locations.push(current);
			}
			running_locations.push(arch.last_index());
			for (size_t i = 0; i < running_locations.length()-2; i++)
			{  
				archetype_index first = running_locations[i];
				archetype_index last= running_locations[i+1];
				archetype_id id = arch.id;
				pool.push([&view, id, start, end, quer]() {
					multi_iter<Components...> iter(view,id, start, end, quer);
					});
			}
		}
	}
	template<typename... Components>
	void multi_query(View<Components...>& view, query_func<Components*...> quer, size_t threads_wanted) {
		size_t total_length = 0;
		for (auto& arch : view.archetypes)
		{
			total_length += (*view.ecs)[arch].count();
		}
		multi_query(view, quer, threads_wanted, 1 + total_length / threads_wanted);

	}
}