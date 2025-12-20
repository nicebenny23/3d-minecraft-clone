#include <shared_mutex>
#include <mutex>   
#pragma once
namespace stn {
	template<typename T>
	class mutex {
	public:
		template<typename Func>
		auto modify(Func&& func) {
			std::unique_lock<std::shared_mutex> l(m);
			return func(value);
		}

		template<typename Func>
		auto read(Func&& func) const {
			std::shared_lock<std::shared_mutex> l(m);
			return func(value);
		}
		template<typename U>
			explicit mutex(U&& initial_value)
			: value(std::forward<U>(initial_value)) {
		}

			template<typename Func>
			auto take(Func&& func)&&{
				std::unique_lock<std::shared_mutex> l(m);
				return func(std::move(value));
			}

	private:
		mutable std::shared_mutex m;
		T value;
	};

}