#include "dynamicarray.h"
#include "thread_pool.h"
#include "../debugger/debug.h"
#pragma once
namespace thread_util {

	//creates range represnetation of a thread split it is inclusive exclusiv
inline Cont::array<size_t> split_many(size_t length, size_t threads) {
		Cont::array<size_t> res;
		size_t total=0;
		for (size_t i = 0; i < threads; i++)
		{
			res.push(total);
			total += length / threads;	
		}
		res.push(length);
		return res;
	}
	template<typename Iterator, typename Func>
	void par_iter(Iterator begin, Iterator end, Func& func, size_t threads) {

		size_t length = std::distance(begin, end);
		if (length == 0) return;
		if (threads > length) threads = length;
		thread::thread_pool thread_pool = thread::thread_pool(threads);

		Cont::array<size_t> size_of_each = thread_util::split_many(length, thread_pool.length());
		Iterator range_start = begin;
		for (size_t i = 0; i < thread_pool.length(); i++)
		{
			Iterator range_next = Iterator(range_start);
			std::advance(range_next, size_of_each[i + 1]-size_of_each[i]);
			thread_pool.push([range_start, range_next, &func]() {
				for (auto it = Iterator(range_start); it != Iterator(range_next); it++) {
					func(*it);
				}
				});
			range_start = range_next;
			if (std::distance(range_start, end) >= size_of_each[i + 1] - size_of_each[i])
			{
				int l = size_of_each[1];
				int z = 2;
			}
		}
	}

}
