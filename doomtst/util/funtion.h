#include <memory>
#pragma once
namespace stn {
	template <typename F, typename R, typename... Args>
	concept StatelessFunction=
		std::is_empty_v<std::remove_cvref_t<F>>&&
		std::is_default_constructible_v<std::remove_cvref_t<F>>&&
		std::is_invocable_r_v<R, F, Args...>;
	template <typename>
	struct Stateless;
	
	
	template <typename R, typename... Args>
	struct Stateless<R(Args...)> {
		Stateless() = delete;
		using ptr_type = R(*)(Args...);
		//apperently the plus allows us to get it to work
		template <typename F>
		constexpr Stateless(F f)requires (StatelessFunction<F,R, Args...>)
			: fn(&call<std::remove_cvref_t<F>>){
		}
		constexpr Stateless(ptr_type f)
			: fn(f) {
		}
		constexpr Stateless& operator=(ptr_type f)
		{
			fn = f;
			return *this;
		}
		template <typename F> requires (StatelessFunction<F,R, Args...>)
		constexpr Stateless& operator=(F f){
			fn = &call<std::remove_cvref_t<F>>;
			return *this;
		}
		bool operator ==(const Stateless& other) const = default;
		bool operator !=(const Stateless& other) const = default;

		constexpr R operator()(Args... args) const{
			return fn(args...);
		}
	private:
		template <typename F>
		static R call(Args... args) {
			if constexpr (std::is_void_v<R>) {
				F{}(std::forward<Args>(args)...);
			}
			else {
				return F{}(std::forward<Args>(args)...);
			}
		}
		ptr_type fn;
	}; 
}