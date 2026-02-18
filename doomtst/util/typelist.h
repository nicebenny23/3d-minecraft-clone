#pragma once
#include <tuple>
#include <type_traits>
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
	using EmptyList = TypeList<>;
	template<typename T>
	inline constexpr bool is_type_list_v = stn::TypeInstantiationOf<T, TypeList>;

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

	template<typename List, typename T>
	struct contains_impl;

	template<typename... Ts, typename T>
	struct contains_impl<TypeList<Ts...>, T>
		: std::bool_constant<(std::same_as<T, Ts> || ...)> {
	};

	template<typename List, typename T>
	inline constexpr bool contains_v= contains_impl<List, T>::value;
	
	template<TypeListType List, typename ...Args>
	struct contains_list_impl;

	template<TypeListType List, typename ...Args>
	struct contains_list_impl<List, TypeList<Args...>>
		: std::bool_constant<(contains_v<List,Args>&&...)> {
	};
	template<TypeListType List, TypeListType Other>
	inline constexpr bool contains_list = contains_list_impl<List, Other>::value;
	
	template<typename List, typename NewType>
	struct append {
		static_assert(is_type_list_v<List>, "append<T>: T must be a TypeList");
	};

	template<typename... Types, typename NewType>
	struct append<TypeList<Types...>, NewType> {
		using type = TypeList<Types..., NewType>;
	};


	template<typename List, typename NewType>
	struct prepend_impl;

	template<typename... Types, typename NewType>
	struct prepend_impl<TypeList<Types...>, NewType> {
		using type = TypeList<NewType, Types...>;
	};

	template<TypeListType List, typename NewType>
	using prepend=typename prepend_impl<List,NewType>::type;


	template<typename List>
	struct remove_first_impl {
	};
	template<typename First, typename ... Rest>
	struct remove_first_impl<TypeList< First, Rest...>> {
		using type = TypeList<Rest...>;
	};

	template<TypeListType List>
	using remove_first_t = typename remove_first_impl<List>::type;

	// --- At ---
	template<std::size_t I, typename List>
	struct at_impl {

		static_assert(I < List::length(), "at_imple<T>: Index out of bounds");
		static_assert(is_type_list_v<List>, "at<T>: T must be a TypeList");
	};

	template<std::size_t I, typename Head, typename... Tail>
	struct at_impl<I, TypeList<Head, Tail...>> : at_impl<I - 1, TypeList<Tail...>> {

	};

	template<typename Head, typename... Tail>
	struct at_impl<0, TypeList<Head, Tail...>> {
		using type = Head;
	};

	// --- at_v alias (for type access) ---
	template<std::size_t I, typename List>
	using at_v = typename at_impl<I, List>::type;

	// --- Find ---

	template<typename List, typename T>
	struct find;

	template<typename... Ts, typename T>
	struct find<TypeList<Ts...>, T> {
	private:
		template<std::size_t... Is>
		static constexpr std::size_t index(std::index_sequence<Is...>) {
			constexpr bool matches[] = { std::same_as<T, Ts>... };
			return ((matches[Is] ? Is : sizeof...(Ts)), ...);
		}

	public:
		static constexpr std::size_t value =
			index(std::index_sequence_for<Ts...>{});
	};
	template<TypeListType List, typename T>
	inline constexpr std::size_t find_v = find<List, T>::value;

	// --- Map ---
	template<typename List, template<typename> class Func>
	struct map {

	};
	template<typename... Ts, template<typename> class F>
	struct map<TypeList<Ts...>, F> {
		using type = TypeList<typename F<Ts>::type...>;
	};
	template<typename List, template<typename> class Func>
	using map_v = typename map<List, Func>::type;
	
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
	template<typename Acc, typename Next>
	struct concat_impl;

	template<typename... AccTs, typename... NextTs>
	struct concat_impl<TypeList<AccTs...>, TypeList<NextTs...>> {
		using type = TypeList<AccTs..., NextTs...>;
	};

	
	template<TypeListType... Lists>
	using concat_all_t = typename fold< TypeList<Lists...>, TypeList<>, concat_impl >::type;
	
	template<typename List, template<typename> class Pred>
	struct filter;

	template<typename... Ts, template<typename> class Pred>
	struct filter<TypeList<Ts...>, Pred> {
	private:
		template<typename T>
		using maybe = std::conditional_t<
			Pred<T>::value,
			TypeList<T>,
			TypeList<>
		>;

	public:
		using type = concat_all_t<maybe<Ts>...>;
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
		: std::bool_constant<(filter_v<List, Pred>::length() == 0)> {
		static_assert(is_type_list_v<List>, "none_satify<T>: T must be a TypeList");

	};
	template<typename List, template<typename> class Pred>
	inline constexpr bool all_satisfy_v = all_satisfy<List, Pred>::value;
	template<typename List, template<typename> class Pred>
	inline constexpr bool none_satify_v = none_satisfy<List, Pred>::value;
	// --- Fold ---
	
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
	struct is_unique_impl : std::true_type {
	};

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
	struct is_unique<TypeList<Ts...>> : is_unique_impl<Ts...> {
	};

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
	using append_unique_v = typename append_unique<List, T>::type;
	template<typename List, typename T>
	struct occurs_once_impl;

	template<typename T, typename... Ts>
	struct occurs_once_impl<TypeList<Ts...>, T>
		: std::bool_constant<((std::is_same_v<T, Ts> ? 1 : 0) + ...) == 1> {
	};

	template<TypeListType List, typename T>
	inline constexpr bool occurs_once = occurs_once_impl<List, T>::value;
	
}
