#include <type_traits>
#include <concepts>
#include <utility>
#pragma once
namespace stn {

	template<typename T>
	constexpr T&& carry(T&& t) noexcept {
		if constexpr (std::is_reference_v<T>) {
			return t;
		}
		else {
			return std::move(t);
		}
	}
	template<typename Func, typename Arg>
	constexpr auto invoke_carry(Func&& f, Arg&& arg)
		-> std::invoke_result_t<Func, Arg> {
		return std::invoke(std::forward<Func>(f), carry(std::forward<Arg>(arg)));
	}
	template<typename Func, typename Arg>
	using invoke_result_ref =
		decltype(std::invoke(std::declval<std::add_lvalue_reference_t<Func>>(),
			std::declval<Arg>()));
	template<typename Func, typename ...Args>
	concept nonvoid_invokable = std::invocable<Func, Args&&...> && !std::is_void_v<std::invoke_result_t<Func, Args&&...>>;
	template<typename>
	struct member_function_traits;
	template<typename C, typename R, typename... Args>
	struct member_function_traits<R(C::*)(Args...)> {
		using return_type = R;
		using args = std::tuple<Args...>;
	};
	template<typename C, typename R, typename... Args>
	struct member_function_traits<R(C::*)(Args...) const> {
		using return_type = R;
		using args = std::tuple<Args...>;
	};
	template<typename Range, typename ElementType>
	concept convertible_range = std::ranges::range<Range> && std::convertible_to<std::ranges::range_value_t<Range>, ElementType>;

}
