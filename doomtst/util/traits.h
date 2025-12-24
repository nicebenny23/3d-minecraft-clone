#include <type_traits>
#include <concepts>
#include <utility>
#pragma once
namespace stn
{

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

}
