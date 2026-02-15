#include <type_traits>
#include <utility>
#include "typelist.h"
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
		Tuple(H&& head, T&&... tail)
			: Base(std::forward<T>(tail)...),
			value(std::forward<H>(head)) {
		}
		Head value;
		using type_list = TypeList::TypeList<Head, Tail...>;
		static constexpr size_t size = type_list::length();
		template<size_t index> requires (index <size)
		using type_at = TypeList::at_v<index,type_list>;

		template<size_t index> requires (index < size)
			auto get(this auto&& self) -> forward_as_member_t<decltype(self), type_at<index>> {
			if constexpr (index == 0) {
				return stn::forward_as_member<decltype(self), Head>(self.value);
			}
			else {
				return static_cast<stn::forward_as_member_t<decltype(self), Base>>(self).template get<index - 1>();
			}
		}
		template<TupleType HeadArgs, TupleType... TailArgs>
		Tuple(HeadArgs&& head_args, TailArgs&&... tail_args)
			:Base(std::forward<TailArgs>(tail_args)...)
			, value(std::apply([](auto&& args...) {
			return Head(std::forward<decltype(args)>(args)...);
				})(std::forward<HeadArgs>(head_args))) {
		}
		template<typename T> 
			requires TypeList::occurs_once<type_list, T>
		auto get(this auto&& self) -> forward_as_member_t<decltype(self), T> {
			return std::forward<decltype(self)>(self).get<TypeList::find_v<type_list, T>>();
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
	template<typename... Ts>
	Tuple(Ts&&...) -> Tuple<std::decay_t<Ts>...>;

	template<typename... Ts>
	constexpr auto make_Tuple(Ts&&... args) {
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
	decltype(auto) get(T&& Tuple) {
		return std::forward<T>(Tuple). template get<Element>();
	}
	template<typename... Types> requires stn::AllUnique<Types...>
	struct TupleSet;

	template<typename T>
	concept TupleSetType = stn::TypeInstantiationOf<T, TupleSet>;
	template<typename ...Args>
	struct Tuple_set_type_list_impl {

	};

	template<typename ...Args>
	struct Tuple_set_type_list_impl<TupleSet<Args...>> {
		using type = TypeList::TypeList<Args...>;
	};
	template<TupleSetType T>
	using tuple_set_type_list = Tuple_set_type_list_impl<T>::type;

	template< typename PotentialSuperset, typename PotentialSubset>
	concept TupleSuperset = TupleSetType<PotentialSuperset> && TupleSetType<PotentialSubset> &&
		TypeList::contains_list<tuple_set_type_list <PotentialSuperset>, tuple_set_type_list <PotentialSubset>>;


	template< typename PotentialSuperset, typename PotentialElement>
	concept TupleSetContains = TupleSetType<PotentialSuperset> &&
		TypeList::contains_v<tuple_set_type_list <PotentialSuperset>, PotentialElement>;


	template<typename ... Types> requires stn::AllUnique<Types...>
	struct TupleSet {

		TupleSet(Types&&... vals) :values(std::forward<Types>(vals)...) {
		}
		using TupleTypeFor = Tuple<Types...>;
		static constexpr size_t size= TupleTypeFor::size;
		template<size_t index> requires (index < size)
		using type_at = typename TupleTypeFor::template type_at<index>;
		bool operator ==(const TupleSet<Types...>& other) const = default;
		bool operator !=(const TupleSet<Types...>& other) const = default;
		template<TupleSetType Other> requires(TupleSuperset<Other, TupleSet<Types...>>)
			TupleSet(Other&& other) :values(get<Types>(std::forward<Other>(other.values))...) {
		}
		template<typename T>
		decltype(auto) get(this auto&& self) {
			return std::forward_like<decltype(self)>(self.values). template get<T>();
		}
		template<size_t N>
		auto get(this auto&& self) -> forward_as_member_t<decltype(self), type_at<N>> {
			return std::forward_like<decltype(self)>(self.values).template get<N>();
		}
	private:
		template<typename ... OtherTypes>   requires stn::AllUnique<OtherTypes...>
		friend struct TupleSet;
		TupleTypeFor values;
	};
	template<size_t N, TupleSetType T>
	auto get(T&& Tuple) -> forward_as_member_t<T&&, typename std::remove_cvref_t<T>::template type_at<N>> {
		return std::forward<T>(Tuple). template get<N>();
	}
	template<TupleSetType T, typename Element>  requires TupleSetContains<T, Element>
	decltype(auto) get(T&& Tuple) {
		return std::forward<T>(Tuple). template get<Element>();
	}

}
namespace std {
	template<typename ...Args>
	struct tuple_size<stn::Tuple<Args...>> : std::integral_constant<std::size_t, sizeof...(Args)> {
	};
	template<size_t N, typename ...Args>
	struct tuple_element<N, stn::Tuple<Args...>> {
		using type = stn::Tuple<Args...>::template type_at<N>;
	};
	template<typename ...Args>
	struct tuple_size<stn::TupleSet<Args...>> : std::integral_constant<std::size_t, sizeof...(Args)> {
	};
	template<size_t N, typename ...Args>
	struct tuple_element<N, stn::TupleSet<Args...>> {
		using type = tuple_element<N,typename stn::TupleSet<Args...>::TupleTypeFor>::type;
	};
}