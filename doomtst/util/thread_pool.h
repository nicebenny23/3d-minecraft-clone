
#include "dynamicarray.h"
#include <functional>
#include <condition_variable>
#include "mathutil.h"
#include <atomic>
#pragma once
namespace thread {
	struct thread_pool_exception : std::exception {
		std::string context;
		std::exception_ptr original;
		thread_pool_exception(std::string ctx, std::exception_ptr ptr):context(ctx),original(ptr) {


		}
		const char* what() const noexcept override {
			return context.c_str();
		}
		void rethrow_original() const {
			if (original) {
				std::rethrow_exception(original);
			}
		}
	};
	struct thread_pool {
		void wait() {
			std::unique_lock lck(mutex);
			
			cv.wait(lck, [this] { return tasks.length == 0 && working_threads.load() == 0; });

			
			stop = true;
			cv.notify_all();
			lck.unlock();

			for (auto& thread : threads) {
				thread.join();
			}
			
		}
	
		using Func = std::function<void()>;
		void thread_wait() {
			while (true)
			{
				
				Func task;
				try {
				{
					std::unique_lock lck(mutex);
					cv.wait(lck, [this] { return stop || !tasks.empty(); });
					if (stop && tasks.empty())
					{
						return;
					}
					if (tasks.empty())
					{
						continue;
					}
					task = std::move(tasks.pop());
				}
					working_threads.fetch_add(1);
					task();
					working_threads.fetch_sub(1);
					{
						std::unique_lock<std::mutex> lck(mutex);
						cv.notify_all();
					}
				}
				catch (...) {
					std::unique_lock lck(mutex);
					except_ptr = std::make_exception_ptr(
						thread_pool_exception(
							"Exception in thread_pool worker thread (task info: ...)",
							std::current_exception()
						)
					);

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
			wait();
		}

		thread_pool(size_t n):tasks(){
			stop = false;
			working_threads = 0;
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
		size_t length() {
return threads.length;

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
