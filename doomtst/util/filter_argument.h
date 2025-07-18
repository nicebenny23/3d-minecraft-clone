#include <tuple>
#include "../util/Not.h"
#include "../util/typelist.h"
#pragma once
namespace type_pipeline {
	template <template<typename> class Predicate, typename... >
	struct FilterImpl;
	template<template< typename > class Predicate>
	struct FilterImpl<Predicate> {
		using type = TypeList::TypeList<>;
	};

	template<template< typename > class Predicate, typename T, typename... Rest>
	struct FilterImpl<Predicate, T, Rest...> {
	private:
		using tail = typename FilterImpl<Predicate, Rest...>::type;

		template<typename... Oth>
		static auto prepend(std::tuple<Oth...>*) -> std::tuple<T, Oth...>;
	public:
		using type = std::conditional_t<
			Predicate<T>::value,
			decltype(prepend((tail*)nullptr)),
			tail
		>;
	};
	template <template<typename> class Predicate, typename... Ts>
	using Filter_t = typename FilterImpl<Predicate, Ts...>::template type<>;
}