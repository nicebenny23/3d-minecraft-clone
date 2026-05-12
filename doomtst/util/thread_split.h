#include "dynamicarray.h"
#include "thread_pool.h"
#include "../debugger/debug.h"
#include "../game/time.h"
#pragma once
namespace thread_util {
    template<typename Iterator, typename Func>
    void par_iter(Iterator begin, Iterator end,Func func, size_t threads) {
        size_t length = std::distance(begin, end);
		if (length == 0) {
			return;
		}
		if (threads > length) {
			threads = length;
		};

		thread::thread_pool thread_pool = thread::thread_pool(threads);
		Iterator range_start = begin;
        for (size_t i = 0; i < thread_pool.length(); i++) {
			size_t skip_count = std::floor(length / threads);
			if (i==thread_pool.length()-1) {
				skip_count = std::distance(range_start,end);
			}
			Iterator range_next = range_start;
			std::advance(range_next, skip_count);
            thread_pool.push([range_start, range_next, func]() {
                for (auto it = Iterator(range_start); it != Iterator(range_next); it++) {
                    func(*it);
                }
            });
            range_start = range_next;
        }
    }
	template<std::ranges::range Rng, typename Func>
	void par_iter_rng(Rng& range, Func func, size_t threads) {
		par_iter(range.begin(), range.end(), func, threads);
	}
}
