#include <memory>
#pragma once
namespace stn {
	template <typename F, typename R, typename... Args>
	concept StatelessFunction =
		std::is_invocable_r_v<R, F, Args...> &&
		std::is_convertible_v<F, R(*)(Args...)>;
	template <typename>
	struct Stateless;

	template <typename R, typename... Args>
	struct Stateless<R(Args...)> {
		Stateless() = delete;
		using ptr_type = R(*)(Args...);
		//apperently the plus allows us to get it to work
		template <typename F>
		constexpr Stateless(F&& f) noexcept requires (StatelessFunction<F,R, Args...>)
			: fn(+f) {
		}

		template <typename F> requires (StatelessFunction<F,R, Args...>)
		constexpr Stateless& operator=(F&& f) noexcept {
			fn = +f;
			return *this;
		}
		constexpr R operator()(Args... args) const noexcept {
			return fn(args...);
		}
	private:
		ptr_type fn;
	};
}