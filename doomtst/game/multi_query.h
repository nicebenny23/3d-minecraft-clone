#include "../util/thread_split.h"
#include "query.h"
#include "../util/thread_pool.h"
#pragma once

template<typename... Components>
using query_func = std::function<void(std::tuple<Components*...>)>;
template<typename... Components>
struct multi_iter {
	query::View<Components...>& owner;
	size_t entity_index;
	size_t arch_index;
	size_t end_index;
	size_t index;
	query_func<Components...> function;

	multi_iter(query::View<Components...>& vw,size_t start,size_t end,const query_func<Components...> func):owner(vw),index(start),end_index(end),function(func) {
		size_t counter=0;
		for (size_t i = 0; i < owner.archtypes.length; i++)
		{
			size_t to_add = owner.archtypes[i]->elems.length;
			if (start <= counter + to_add)
			{
				arch_index = i;
				entity_index = start - counter;
				return;
			}
			counter += to_add;
		}

	}
	
	void iterate() {
		while (index<end_index)
		{
			apply();
			inc();
		}

	}
	private:
		inline void apply() {
			auto& arch_elems= owner.archtypes[arch_index]->elems;
			function(owner.ecs->get_tuple<Components...>(arch_elems[entity_index], owner.positions));

		}
		inline void inc() {
			entity_index++;

			// loop until we find a valid entity in a valid archetype
			while (arch_index < owner.archtypes.length) {
				auto& arch = owner.archtypes[arch_index];
				if (entity_index < arch->elems.length) {
					break;
				}
				arch_index++;
				entity_index = 0;
			}
			index++;

		}
}; 
template<typename... Components>
void multi_query(query::View<Components...>& view, std::function<void(std::tuple<Components*...>)> quer,size_t threads_wanted) {
	size_t total_length = 0;

	for (auto arch : view.archtypes)
	{
		total_length += arch->elems.length;
	}
	if (threads_wanted > total_length) {
		threads_wanted = total_length;
	}
	thread::thread_pool pool(threads_wanted);
	

	Cont::array<size_t> size_of_each = thread_util::split_many(total_length, pool.length());
	for (size_t i = 0; i < pool.length(); i++)
	{
		size_t start= size_of_each[i];
		size_t end= size_of_each[i+1];
		pool.push([&view, quer, start, end]() {
			multi_iter<Components...> iter(view, start, end, quer);
			iter.iterate();
			});
	}


}

template<typename... Components>
void multi_query(query::View<Components...>& view, std::function<void(std::tuple<Components*...>)> quer, size_t threads_wanted,size_t count) {
	size_t total_length = 0;

	for (auto arch : view.archtypes)
	{
		total_length += arch->elems.length;
	}
	if (threads_wanted > total_length) {
		threads_wanted = total_length;
	}
	thread::thread_pool pool(threads_wanted);


	Cont::array<size_t> size_of_each = thread_util::split_many(total_length, total_length/count);
	for (size_t i = 0; i < size_of_each.length-1; i++)
	{
		size_t start = size_of_each[i];
		size_t end = size_of_each[i + 1];
		pool.push([&view, quer, start, end]() {
			multi_iter<Components...> iter(view, start, end, quer);
			iter.iterate();
			});
	}


}