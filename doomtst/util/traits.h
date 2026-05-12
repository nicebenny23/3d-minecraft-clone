#include <type_traits>
#include <concepts>
#include <utility>
#include "concepts.h"
#include "typelist.h"
#pragma once
namespace stn {
	template<bool is_const,typename T>
	using apply_const_t = std::conditional_t<is_const, const T,T>;
	template<typename Like, typename T>
	using apply_const_like_t = apply_const_t<std::is_const_v<std::remove_reference_t<Like>>, T>;
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
	
	//todo move up
	template<typename>
	struct function_traits;
	template<typename C, typename R, typename... Args>
	struct function_traits<R(C::*)(Args...)> {
		using return_type = R;
		using args = std::tuple<Args...>;
	};
	template<typename C, typename R, typename... Args>
	struct function_traits<R(C::*)(Args...) const> {
		using return_type = R;
		using args = std::tuple<Args...>;
	};
	template<typename R, typename... Args>
	struct function_traits<R(Args...)> {
		using return_type = R;
		using args = std::tuple<Args...>;
	};
	template<typename R, typename... Args>
	struct function_traits<R(*)(Args...)>
		: function_traits<R(Args...)> {
	};
	template<typename T>
	concept HasFunctionTraits= requires {
		typename function_traits<T>::return_type;
		typename function_traits<T>::args;
	};
	//this is shockingly not provided in std
	template<typename Owner, typename Member>
	using forward_as_member_t = std::conditional_t<
		std::is_reference_v<Member>,
		Member,
		decltype(std::forward_like<Owner>(std::declval<Member>()))
	>;

	template<typename Owner, typename Member>
	constexpr auto forward_as_member(std::remove_reference_t<Member>& value) noexcept
		-> forward_as_member_t<Owner, Member> {
		return static_cast<forward_as_member_t<Owner, Member>>(value);
	}
	template<typename T, typename Owner>
	constexpr decltype(auto) forward_cast(Owner&& owner) noexcept {
		return std::forward_like<Owner>(static_cast<T&>(owner));
	}
	template<HasFunctionTraits T,std::size_t N>
	using arg_at_t = std::tuple_element_t<N, typename function_traits<T>::args>;

	template<HasFunctionTraits T>
	using arguments_t = typename function_traits<T>::args;

	template<HasFunctionTraits T>
	using result_type_t = typename function_traits<T>::return_type;

	template <typename T, template <typename...> class Template>
	struct type_instantiation_parameters_impl {
		using types = TypeList::EmptyList;
	};

	
	template <template <typename...> class Template, typename... Args>
	struct type_instantiation_parameters_impl<Template<Args...>, Template> {
		using types = TypeList::TypeList<Args...>;
	};

	template <typename T, template <typename...> class Template>
		requires stn::TypeInstantiationOf<T, Template>
	using type_instantiation_parameters = typename type_instantiation_parameters_impl<T, Template>::types;

	template <template <typename...> class Template,typename TypeList>
	struct instantiate_with_list_parameters_impl {
	};

	template <template <typename...> class Template, typename... Args>
	struct instantiate_with_list_parameters_impl<Template,TypeList::TypeList<Args...>> {
		using type = Template<Args...>;
	};

	template <template <typename...> class Template,TypeList::TypeListType T>
	using instantiate_with_list= typename instantiate_with_list_parameters_impl<Template,T>::type;

	template<typename Range, typename ElementType>
	concept convertible_range = std::ranges::range<Range> && std::convertible_to<std::ranges::range_value_t<Range>, ElementType>;
}
