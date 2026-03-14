#include <type_traits>
#include <utility>
#include "typelist.h"
#include <tuple>
#include "concepts.h"
#include "traits.h"
#pragma once
namespace stn {

	template<typename... Types>
	struct Tuple;

	template<typename T>
	concept TupleType = stn::TypeInstantiationOf<T, Tuple>;

	template<>
	struct Tuple<> {

		using type_list = TypeList::TypeList<>;
		bool operator==(const Tuple<>&) const {
			return true;
		}
		bool operator!=(const Tuple<>&) const {
			return false;
		}

		Tuple() {

		}
	};


	template<typename Head, typename...Tail>
	struct Tuple<Head, Tail...> :Tuple<Tail...> {

		using Base = Tuple<Tail...>;

		template<typename H, typename... T>
		Tuple(H&& head, T&&... tail)			requires (std::constructible_from<Head, H&&>)
			: Base(std::forward<T>(tail)...),
			value(std::forward<H>(head)) {
		}
		template<typename Arg>
			requires (std::constructible_from<Head, Arg> &&std::constructible_from<Base, Arg>)
			Tuple(Arg&& arg): Base(std::forward<Arg>(arg)), value(std::forward<Arg>(arg)) {
		}
		Head value;
		using type_list = TypeList::TypeList<Head, Tail...>;
		static constexpr size_t size = type_list::length();
		template<size_t index> requires (index <size)
		using type_at = TypeList::at_v<index,type_list>;

		template<size_t index> requires (index < size)
			auto get(this auto&& self) -> forward_as_member_t<decltype(self), type_at<index>> {
			if constexpr (index == 0) {
				return std::forward<decltype(self)>(self).value;
			}
			else {
				return static_cast<stn::forward_as_member_t<decltype(self), Base>>(self).template get<index - 1>();
			}
			
		}
		template<typename T>
			requires TypeList::occurs_once<type_list, T>
		auto get(this auto&& self) -> forward_as_member_t<decltype(self), T> {
			if constexpr (std::same_as<T, Head>) {
				return std::forward<decltype(self)>(self).value;
			}
			else {
				return static_cast<forward_as_member_t<decltype(self), Base>>(self)
					.template get<T>();
			}
		}

		template<TupleType HeadArgs, TupleType... TailArgs>
		Tuple(HeadArgs&& head_args, TailArgs&&... tail_args)
			:Base(std::forward<TailArgs>(tail_args)...)
			, value(std::apply([](auto&& args...) {
			return Head(std::forward<decltype(args)>(args)...);
				})(std::forward<HeadArgs>(head_args))) {
		}

		friend constexpr bool operator==(const Tuple& first, const Tuple& second)
			requires (std::equality_comparable<Head> &&std::equality_comparable<Base>) {
			return first.value == second.value &&
				static_cast<const Base&>(first)==static_cast<const Base&>(second);
		}
		friend constexpr bool operator!=(const Tuple& first, const Tuple& second)  requires std::equality_comparable<Base>&& std::equality_comparable<Head> {
			return !(first == second);
		}
	};
	template<typename F, typename... Ts>
	constexpr auto map_tuple(const stn::Tuple<Ts...>& t, F&& f) {
		return std::apply([&f](const Ts&... args) {
			return stn::Tuple{ f(args)... }; 
		}, t);
	}
	template<typename F, typename... Ts>
	bool all_tuple(const stn::Tuple<Ts...>& t, F&& f) {
		return std::apply([&f](const Ts&... args) {
			return (f(args) && ...);  
			}, t);
	}
	template<typename... Ts>
	Tuple(Ts&&...) -> Tuple<Ts&&...>;

	template<typename... Ts>
	constexpr auto make_tuple(Ts&&... args) {
		return Tuple<std::decay_t<Ts>...>(std::forward<Ts>(args)...);
	}
	template<typename Tup, typename Included>
	concept TupleIncludesUniquely = TupleType<Tup> && requires(Tup & value) {
		value.template get<Included>;
	};

	template<size_t N,TupleType T>
	auto get(T&& Tuple) -> forward_as_member_t<T&&, typename T::template type_at<N>> {
		return std::forward<T>(Tuple). template get<N>();
	}
	template<TupleType T, typename Element>  requires TupleIncludesUniquely<T, Element>
	auto get(T&& Tuple) -> forward_as_member_t<T&&, Element> {
		return std::forward<T>(Tuple). template get<Element>();
	}
	template<typename... Types> requires stn::AllUnique<Types...>
	struct TupleSet;

	template<typename T>
	concept TupleSetType = stn::TypeInstantiationOf<T, TupleSet>;
	
	template<TupleSetType T>
	using tuple_set_type_list =stn::type_instantiation_parameters<T,TupleSet>;

	template< typename PotentialSuperset, typename PotentialSubset>
	concept TupleSuperset = TupleSetType<PotentialSuperset> && TupleSetType<PotentialSubset> &&
		TypeList::contains_list<tuple_set_type_list <PotentialSuperset>, tuple_set_type_list <PotentialSubset>>;


	template< typename PotentialSuperset, typename PotentialElement>
	concept TupleSetContains = TupleSetType<PotentialSuperset> &&
		TypeList::contains_v<tuple_set_type_list <PotentialSuperset>, PotentialElement>;

	template<TypeList::TypeListType TypeListType>
	using tuple_set_from_typelist = instantiate_with_list<TupleSet, TypeListType>;
	template<TupleSetType T, template<typename> class Pred>
	using subset_passing = tuple_set_from_typelist<TypeList::filter_v<tuple_set_type_list<T>,Pred>>;

	template<typename T>
	struct SetReducer {
		
		SetReducer(T&& value) :reduction(std::move(value)) {
		};
		SetReducer(const T& value) requires std::copy_constructible<T>:reduction(value) {
		};
		using held_type = T;
		T reduction;
	};
	template<typename T>
	concept ReducerType = stn::TypeInstantiationOf<T, SetReducer>;
	
	template<ReducerType T>
	using reducer_held_type = typename T::held_type;

	template<typename ... Types> requires stn::AllUnique<Types...>
	struct TupleSet {
	
		TupleSet(Types&&... vals) :values(std::forward<Types>(vals)...) {
		}
		
		using TupleTypeFor = Tuple<Types...>;
		template<typename Arg> requires std::constructible_from<TupleTypeFor,Arg>
		TupleSet(Arg&& arg) :values(std::forward<Arg>(arg)) {
		}
		static constexpr size_t size= sizeof...(Types);
		template<size_t index> requires (index < size)
		using type_at = TupleTypeFor::template type_at<index>;
		bool operator ==(const TupleSet& other) const requires(std::equality_comparable<TupleTypeFor>){
			return values == other.values;
		};
		bool operator !=(const TupleSet& other) const requires(std::equality_comparable<TupleTypeFor>) {
			return values != other.values;
		};
		template<ReducerType Other> requires TupleSetType<reducer_held_type<Other>>&&TupleSuperset<reducer_held_type<Other>, TupleSet>
			TupleSet(Other&& other) :values(get<Types>(std::forward<typename Other::tuple_set_type >(other.reduction.values))...) {
		}
		SetReducer<TupleSet> reduce(this auto&& self) {
			return SetReducer<TupleSet>(std::forward<decltype(self)>(self));
		}
		template<typename T>
		auto get(this auto&& self) -> forward_as_member_t<decltype(self), T> {
			return std::forward_like<decltype(self)>(self.values). template get<T>();
		}
		template<size_t N>  requires (N < size)
		auto get(this auto&& self) -> forward_as_member_t<decltype(self), type_at<N>> {
			return std::forward_like<decltype(self)>(self.values).template get<N>();
		}
		private:
			template<typename ... OtherTypes>   requires stn::AllUnique<OtherTypes...>
			friend struct TupleSet;
			TupleTypeFor values;
	};
	template<typename... Ts>
	TupleSet(Ts&&...) -> TupleSet<Ts&&...>;
	template<size_t N, TupleSetType T>
	auto get(T&& Tuple) -> forward_as_member_t<T&&, typename std::remove_cvref_t<T>::template type_at<N>> {
		return std::forward<T>(Tuple). template get<N>();
	}
	template<TupleSetType T, typename Element>  requires TupleSetContains<T, Element>
	auto get(T&& Tuple)->forward_as_member_t<T&&, Element>{
		return std::forward<T>(Tuple). template get<Element>();
	}
	template<typename... Ts>
	TupleSet<Ts&...>  ref_set(Ts&... args) {
		return TupleSet<Ts&...>(args...);
	}
	//this function was ai generated
	template<typename F, typename... Ts>
	constexpr auto map_tuple(stn::TupleSet<Ts...>& t, F&& f)
		-> stn::TupleSet<decltype(f(std::declval<Ts&>()))...> {
		return[&]<size_t... Is>(std::index_sequence<Is...>) {
			return stn::TupleSet<decltype(f(t.template get<Is>()))...>(
				f(t.template get<Is>())...
			);
		}(std::index_sequence_for<Ts...>{});
	}

	template<typename F, typename... Ts>
	bool all_tuple(const stn::TupleSet<Ts...>& t, F&& f) {
		return[&]<size_t... Is>(std::index_sequence<Is...>) {
			return (f(t.template get<Is>()) && ...);
		}(std::index_sequence_for<Ts...>{});
	}

}
namespace std {
	template <std::size_t I, typename... Ts>
	auto get(stn::Tuple<Ts...>& t) -> decltype(stn::get<I>(t)) {
		return stn::get<I>(t);
	}
	
	template <std::size_t I, typename... Ts>
	auto get(const stn::Tuple<Ts...>& t) -> decltype(stn::get<I>(t)) {
		return stn::get<I>(t);
	}

	template <std::size_t I, typename... Ts>
	auto get(stn::Tuple<Ts...>&& t) -> decltype(std::move(stn::get<I>(t))) {
		return std::move(stn::get<I>(t));
	}

	template <std::size_t I, typename... Ts>
	auto get(stn::TupleSet<Ts...>& s) -> decltype(stn::get<I>(s)) {
		return stn::get<I>(s);
	}

	template <std::size_t I, typename... Ts>
	auto get(const stn::TupleSet<Ts...>& s) -> decltype(stn::get<I>(s)) {
		return stn::get<I>(s);
	}

	template <std::size_t I, typename... Ts>
	auto get(stn::TupleSet<Ts...>&& s) -> decltype(std::move(stn::get<I>(s))) {
		return std::move(stn::get<I>(s));
	}

	template<typename ...Args>
	struct tuple_size<stn::Tuple<Args...>> : std::integral_constant<std::size_t, sizeof...(Args)> {
	};
	template<size_t N, typename ...Args>
	struct tuple_element<N, stn::Tuple<Args...>> {
		using type = typename stn::Tuple<Args...>::template type_at<N>;
	};
	template<typename ...Args>
	struct tuple_size<stn::TupleSet<Args...>> : std::integral_constant<std::size_t, sizeof...(Args)> {
	};
	template<size_t N, typename ...Args>
	struct tuple_element<N, stn::TupleSet<Args...>> {
		using type = typename tuple_element<N,typename stn::TupleSet<Args...>::TupleTypeFor>::type;
	};
}