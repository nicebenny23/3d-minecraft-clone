#pragma once

#include <type_traits>
#include <cstddef>
#include "concepts.h"
namespace TypeList {

    // --- TypeList container ---
    template<typename... Types>
    struct TypeList {
        //length utility on the typelist to reduce verbosity
        static constexpr std::size_t length() noexcept {
            return sizeof...(Types);
        }

        //empty utility on the typelist to reduce verbosity
        static constexpr bool empty() noexcept {
            return sizeof...(Types) == 0;
        }


    };
    using None = TypeList<>;
	
    // Helper: is_type_list trait
    template<typename T>
    struct is_type_list : std::false_type {};

    template<typename... Ts>
    struct is_type_list<TypeList<Ts...>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_type_list_v = is_type_list<T>::value;

	template<typename T>
	concept TypeListType = is_type_list_v<T>;

	template<typename T>
	struct type_list_to_tuple;
	template<typename ...Args>
	struct type_list_to_tuple < TypeList<Args...>> {
		using type = std::tuple<Args...>;
	};

	template<TypeListType List>
	using type_list_to_tuple_t = typename type_list_to_tuple<List>::type;
	template<typename T>
	struct type_list_from_tuple;
	template<typename ...Args>
	struct type_list_from_tuple <std::tuple<Args...>> {
		using type = TypeList<Args...>;
	};

	//need to ensure it is tuple
	template<typename List>
	using type_list_from_tuple_t = typename type_list_from_tuple<List>::type;
    // --- Length ---
    template<typename List>
    struct length
    {
        static_assert(is_type_list_v<List>, "len<T>: T must be a TypeList");
    };

    template<typename... Types>
    struct length<TypeList<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)> {};

    template<typename List>
    inline constexpr std::size_t length_v = length<List>::value;

    template<typename List>
    struct is_empty {
        static_assert(is_type_list_v<List>, "is_empty<T>: T must be a TypeList");
    };

    template<typename... Types>
    struct is_empty<TypeList<Types...>>
        : std::bool_constant<(length_v<TypeList<Types...> > == 0)> {
    };
    template<typename List>
    inline constexpr bool is_empty_v = is_empty<List>::value;

    // --- Contains ---
    template<typename List, typename T>
    struct contains {
        static_assert(is_type_list_v<List>, "contains<T>: T must be a TypeList");
    };

    template<typename T>
    struct contains<TypeList<>, T> : std::false_type {};

    template<typename Head, typename... Tail, typename T>
    struct contains<TypeList<Head, Tail...>, T>
        : std::conditional_t<std::is_same_v<Head, T>, std::true_type, contains<TypeList<Tail...>, T>> {
    };

    template<typename List, typename T>
    inline constexpr bool contains_v = contains<List, T>::value;

    // --- Append ---
    template<typename List, typename NewType>
    struct append {
        static_assert(is_type_list_v<List>, "append<T>: T must be a TypeList");
    };

    template<typename... Types, typename NewType>
    struct append<TypeList<Types...>, NewType> {
        using type = TypeList<Types..., NewType>;
    };


    template<typename List, typename NewType>
    struct prepend {
        static_assert(is_type_list_v<List>, "prepend<T>: T must be a TypeList");
    };

	template<typename... Types, typename NewType>
	struct prepend<TypeList<Types...>, NewType> {
		using type = TypeList<NewType, Types...>;
	};

	template<typename List>
	struct remove_first{
	};
	template<typename First, typename ... Rest>
	struct remove_first<TypeList< First,Rest...>> {
		using type = TypeList<Rest...>;
	};

	template<TypeListType List>
	using remove_first_t =typename remove_first<List>::type;

    // --- At ---
    template<std::size_t I, typename List>
    struct at {

        static_assert(I < length_v<List>, "at<T>: Index out of bounds");
        static_assert(is_type_list_v<List>, "at<T>: T must be a TypeList");
    };

    template<std::size_t I, typename Head, typename... Tail>
    struct at<I, TypeList<Head, Tail...>> : at<I - 1, TypeList<Tail...>> {

    };

    template<typename Head, typename... Tail>
    struct at<0, TypeList<Head, Tail...>> {
        using type = Head;
    };

    // --- at_v alias (for type access) ---
    template<std::size_t I, typename List>
    using at_v = typename at<I, List>::type;

    // --- FindIndex (internal helper) ---
    template<typename T, typename... Ts>
    struct find_index;

    template<typename T, typename... Tail>
    struct find_index<T, T, Tail...> {
        static constexpr std::size_t value = 0;
    };

    // Recursive case: T != Head
    template<typename T, typename Head, typename... Tail>
    struct find_index<T, Head, Tail...> {
        static constexpr std::size_t value = 1 + find_index<T, Tail...>::value;
    };
    template<typename T>
    struct find_index<T> {

        static constexpr std::size_t value = 0;
        static_assert([] { return false; }(), "Type not found in TypeList");
    };

    // --- Find ---
    template<typename List, typename T>
    struct find {
        static_assert(is_type_list_v<List>, "find<T>: T must be a TypeList");
    };

    template<typename... Ts, typename T>
    struct find<TypeList<Ts...>, T> {
        static constexpr std::size_t value = find_index<T, Ts...>::value;
    };

    template<typename List, typename T>
    inline constexpr std::size_t find_v = find<List, T>::value;

    // --- Map ---
    template<typename List, template<typename> class Func>
    struct map {
        static_assert(is_type_list_v<List>, "map<T>: T must be a TypeList");
    };

    template<template<typename> class Func>
    struct map<TypeList<>, Func> {
        using type = TypeList<>;
    };

    template<typename Head, typename... Tail, template<typename> class Func>
    struct map<TypeList<Head, Tail...>, Func> {
        using type = typename prepend<
            typename map<TypeList<Tail...>, Func>::type,
            typename Func<Head>::type
        >::type;
    };
    template<typename List, template<typename> class Func>
    using map_v = typename map<List, Func>::type;
    // --- Filter ---
    template<typename List, template<typename> class Pred>
    struct filter {
        static_assert(is_type_list_v<List>, "filter<T>: T must be a TypeList");
    };

    template<template<typename> class Pred>
    struct filter<TypeList<>, Pred> {
        using type = TypeList<>;
    };

    template<typename Head, typename... Tail, template<typename> class Pred>
    struct filter<TypeList<Head, Tail...>, Pred> {
        using type = std::conditional_t<
            Pred<Head>::value,
            typename prepend<typename filter<TypeList<Tail...>, Pred>::type, Head>::type,
            typename filter<TypeList<Tail...>, Pred>::type
        >;
    };
    template<typename List, template<typename> class Pred>
    using filter_v = typename filter<List, Pred>::type;
    template<typename List, template<typename> class Pred>
    struct all_satisfy
        : std::bool_constant<filter_v<List, Pred>::length() == List::length()> {
        static_assert(is_type_list_v<List>, "all_satify<T>: T must be a TypeList");

    };
    template<typename List, template<typename> class Pred>
    struct none_satisfy
        : std::bool_constant<(length_v<filter_v<List, Pred>> == 0)> {
        static_assert(is_type_list_v<List>, "none_satify<T>: T must be a TypeList");

    };
    template<typename List, template<typename> class Pred>
    inline constexpr bool all_satisfy_v = all_satisfy<List, Pred>::value;
    template<typename List, template<typename> class Pred>
    inline constexpr bool none_satify_v = none_satisfy<List, Pred>::value;
    // --- Fold ---
    template<typename List, typename Init, template<typename Acc, typename T> class Func>
    struct fold {
        static_assert(is_type_list_v<List>, "fold<T>: T must be a TypeList");
    };

    template<typename Init, template<typename Acc, typename T> class Func>
    struct fold<TypeList<>, Init, Func> {
        using type = Init;
    };

    template<typename Head, typename... Tail, typename Init, template<typename Acc, typename T> class Func>
    struct fold<TypeList<Head, Tail...>, Init, Func> {
        using type = typename fold<
            TypeList<Tail...>,
            typename Func<Init, Head>::type,
            Func
        >::type;
    };
    template<typename List, typename Init, template<typename Acc, typename T> class Func>
    using fold_v = typename fold<List, Init, Func>::type;
    // --- for_each (runtime) ---
    template<typename List>
    struct for_each {

        static_assert(is_type_list_v<List>, "for_each<T>: T must be a TypeList");

    };

    template<typename... Types>
    struct for_each<TypeList<Types...>> {
        template<typename F, typename... Args>
        static void apply(Args&&... args) {
            (F::template call<Types>(std::forward<Args>(args)...), ...);
        }
    };

    // --- is_unique ---
    template<typename... Ts>
    struct is_unique_impl : std::true_type {};

    template<typename T, typename... Ts>
    struct is_unique_impl<T, Ts...> : std::conditional_t<
        contains_v<TypeList<Ts...>, T>,
        std::false_type,
        is_unique_impl<Ts...>
    > {
    };

    template<typename List>
    struct is_unique;

    template<typename... Ts>
    struct is_unique<TypeList<Ts...>> : is_unique_impl<Ts...> {};

    template<typename List>
    inline constexpr bool is_unique_v = is_unique<List>::value;
    template<typename List, typename T>
    struct append_unique {
        using type = std::conditional_t<
            contains_v<List, T>,
            List,
            typename append<List, T>::type
        >;
    };
    template<typename List, typename T>
    using append_unique_v = typename append_unique<List,T>::type;

    template<typename... Lists>
    struct concat_all;

    // zero lists -> empty list
    template<>
    struct concat_all<> {
        using type = TypeList<>;
    };

    // single list -> itself
    template<typename List>
    struct concat_all<List> {
        using type = List;
    };

    // recursive: fold the remaining lists into Head using append
    template<typename Head, typename... Tail>
    struct concat_all<Head, Tail...> {
        using type = typename fold<
            TypeList<Tail...>, // the list of lists to iterate
            Head,              // accumulator starts as Head
            append             // append<Acc, T>::type produces Acc with T appended
        >::type;
    };

    template<typename... Lists>
    using concat_all_t = typename concat_all<Lists...>::type;

}
