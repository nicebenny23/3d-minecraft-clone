/*include <thread>
#include "dynamicarray.h"
#include <functional>
#include <condition_variable>
#include "mathutil.h"
#include <>
#pragma once
struct thread_pool {
	void thread_wait() {
		std::unique_lock<





	}
	using Func = std::function< std::function<void()>>;
	thread_pool(size_t n) {

		size_t max_threads = Min(n,std::thread::hardware_concurrency());
		for ( i = 0; i < length; i++)
		{
			
		}

	}
	void push(Func task) {

	
	}
private:
	Cont::array<std::thread> threads;
	std::condition_variable cv;
		Cont::array<Func> tasks;
};
*/