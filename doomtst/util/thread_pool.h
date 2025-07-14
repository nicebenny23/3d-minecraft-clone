
#include "dynamicarray.h"
#include <functional>
#include <condition_variable>
#include "mathutil.h"
#include <atomic>
#pragma once
namespace thread {
	struct thread_pool {


		using Func = std::function<void()>;
		void thread_wait() {
			while (true)
			{
				Func task;

				{
					std::unique_lock lck(mutex);
					cv.wait(lck, [this] { stop || !threads.empty(); });
					if (stop && threads.empty())
					{
						return;
					}
					task = std::move(tasks.pop());
				}
				try {
					working_threads++;
					task();
					working_threads;
				}
				catch (...) {
					std::unique_lock lck(mutex);
					except_ptr = std::current_exception();
				}
			}
		}
		void check_exeption() {
			if (except_ptr != nullptr)
			{
				std::unique_lock lck(mutex);
				std::rethrow_exception(except_ptr);
			}

		}
		bool has_work() {

			return tasks.length != 0||working_threads!=0;
		}
		~thread_pool() {
			{
				std::unique_lock lck(mutex);
				stop = true;
			}
			cv.notify_all();
			for (auto& thread : threads)
			{
				thread.join();
			}
		}

		thread_pool(size_t n) {

			size_t max_threads = Min(n, std::thread::hardware_concurrency());
			for (size_t i = 0; i < max_threads; i++)
			{
				threads.push(std::thread([this] {thread_wait(); }));
			}

		}
		void push(Func task) {
			check_exeption();
			std::unique_lock lck(mutex);
			tasks.push(task);
			cv.notify_one();
		}
	private:
		Cont::array<std::thread> threads;
		std::condition_variable cv;
		Cont::array<Func> tasks;
		std::mutex mutex;
		bool stop;
		std::exception_ptr except_ptr;
		std::atomic<size_t> working_threads;
	};
}
