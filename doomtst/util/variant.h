#pragma once
#include "erased.h"
#include "typelist.h"
#include <algorithm>
#include <variant>
#include <stdexcept>
#include "invoke.h"
#include "layout.h"
#include <type_traits>
#include <utility>
#pragma once

namespace stn {

	struct mono {
		bool operator==(const mono& oth) const {
			return true;
		}
		bool operator!=(const mono& oth) const {
			return false;
		}
		mono() = default;

		mono(const mono& oth) = default;
	};
	//optimized to a switch
	//must have one argument othewise it tries to implicitly form the template paramater
	template <std::size_t N, typename Func, typename... Args>
	auto switch_index(std::size_t index, Func&& func, Args&&... args) -> decltype(func.template operator() < N - 1 > (std::forward<Args>(args)...))  requires(N > 0) {

		constexpr size_t lower = N - 1;
		if (index == lower) {
			return func.template operator() < lower > (std::forward<Args>(args)...);
		}
		else if constexpr (lower != 0) {
			return switch_index<lower>(index, std::forward<Func>(func), std::forward<Args>(args)...);
		}
		else {
			throw std::logic_error("Index Out of range");
		}
	}

	template<typename T>
	struct typed {
		T&& value;
		explicit typed(T&& v) : value(std::forward<T>(v)) {
		}
		typed& operator=(const typed& other) {
			value = other.value;
			return *this;
		}
		typed& operator=(typed&& other) noexcept {
			value = std::move(other.value);
			return *this;
		}
		friend bool operator==(const typed& lhs, const typed& rhs) {
			return lhs.value == rhs.value;
		}

		friend bool operator!=(const typed& lhs, const typed& rhs) {
			return !(lhs == rhs);
		}
		typed(typed&& other) noexcept {
			value = std::move(other.value);

		}
		typed(const typed& other) {
			value = other.value;

		}
	}; template <typename T>
		struct is_equality_comparable : std::bool_constant<std::equality_comparable<T>> {
	};
	template<typename ...types>
	struct variant {

		using type_list = TypeList::TypeList<types...>;

		static constexpr size_t variants = type_list::length();

		static_assert(TypeList::none_satify_v< type_list, std::is_reference>, "variant<Types...> prohibits const types");
		static_assert(TypeList::none_satify_v< type_list, std::is_const>, "variant<Types...> prohibits const types");
		static_assert(TypeList::none_satify_v< type_list, std::is_void>, "variant<Types...> prohibits void types");
		static_assert(TypeList::is_unique_v<type_list>, "variant<Types...> requires all types to be unique");
		template<typename T>
		static constexpr bool is_member = TypeList::contains_v<type_list, T>;
		template<typename T>
		consteval char index_of() {

			static_assert(TypeList::contains_v<type_list, T>, "Type T is not in this TypeList");
			return TypeList::find_v<type_list, T>;
		}
		consteval bool valid_index(size_t index) {
			return index < variants;
		}

		template<size_t index> requires (valid_index(index))
			using type_at = typename TypeList::at_v<index, type_list>;
		variant(variant& other) {
			copy_from(other);
		}

		variant& operator=(variant& other) {

			if (this != &other)
				copy_from(other);
			return *this;
		}

		variant(variant&& other) noexcept {
			move_from(std::move(other));
		}

		variant& operator=(variant&& other) noexcept {
			if (this != &other)
				move_from(std::move(other));
			return *this;
		}

		bool operator==(const variant& other) const requires(TypeList::all_satisfy_v<type_list, is_equality_comparable>) {

			if (index != other.index) {
				return false;
			}
			return	(other).match([](const auto& value, const auto& us) {
				using T = std::remove_cvref_t<decltype(value)>;
				return value == us.get<T>();
				}, *this);
		}
		bool operator!=(const variant& other) const requires(TypeList::all_satisfy_v<type_list, is_equality_comparable>) {
			return !(*this == other);
		}
		template <typename T>
		variant(typed<T> value) requires is_member<T> {


			storage.construct<T>(std::forward<T>(value.value));
			index = index_of<T>();
		}
		template <typename T>
		variant operator=(typed<T> value) requires is_member<T> {

			if (index != index_of<T>()) {
				clear();
				storage.construct<T>(std::forward<T>(value.value));
			}
			else {
				storage.set(std::forward<T>(value.value));
			}
			index = index_of<T>();

		}

		template <typename T>
		void set(std::type_identity<T>::type&& value)  requires is_member<T> {
			//swaped types case
			if (index != index_of<T>()) {
				clear();
				storage.construct<T>(std::forward<T>(value));
			}
			else {
				storage.set(std::forward<T>(value));
			}
			index = index_of<T>();
		}

		template <typename T>
		T& get() requires is_member<T> {

			if (index != index_of<T>()) {
				throw std::logic_error("Attempted to get an inactive element of a variant");
			}
			return storage.template get<T>();
		}
		template <typename T>
		const T& get() const requires is_member<T> {
			if (index != index_of<T>()) {
				throw std::logic_error("Attempted to get an inactive element of a variant");
			}
			return storage.template get<T>();
		}
		template <typename T>
		bool is() const requires is_member<T> {
			return (index == index_of<T>());
		}
		template <typename Func, typename TypeList, typename... Args>
		struct visitor_invocability_checker;

		template <typename Func, typename... Ts, typename... Args>
		struct visitor_invocability_checker<Func, TypeList::TypeList<Ts...>, Args...> {
		private:
			template <typename T>
			static consteval void check_single() {

				static_assert(std::is_invocable<Func, T&, Args...>(), "Visitor cannot be invoked with the given argument types" __FUNCSIG__);
			}

		public:
			static consteval void check_all() {
				(check_single<Ts>(), ...);
			}
		};


		friend struct visit_invoker;
		template <typename VariantType, typename Func, typename ...Args>
		struct visit_invoker {
			using type_list = typename VariantType::type_list;


			Func&& func;
			VariantType* var;
			std::tuple<Args...> args;
			visit_invoker(VariantType* v, Func&& f, Args&&... arguments)
				: func(std::forward<Func>(f)), var(v), args(std::forward_as_tuple(std::forward<Args>(arguments)...)) {

			}
			decltype(auto) visit() {
				return switch_index<variants>(var->index, *this);
			};

			template <size_t I, size_t... Is>
			decltype(auto) call_with_args(std::index_sequence<Is...>) {
				using T = type_at<I>;
				return std::forward<Func>(func)(var->template get<T>(), std::forward<Args>(std::get<Is>(args))...);
			}

			template <size_t I>
			decltype(auto) operator()() requires(valid_index(I)) {
				return call_with_args<I>(std::make_index_sequence<sizeof...(Args)>{});
			}

			template <size_t I, size_t... Is>
			decltype(auto) call_with_args(std::index_sequence<Is...>) const {
				using T = type_at<I>;
				return std::forward<Func>(func)(var->template get<T>(), std::forward<Args>(std::get<Is>(args))...);
			}

			template <size_t I>
			decltype(auto) operator()()  const  requires(valid_index(I)) {
				return call_with_args<I>(std::make_index_sequence<sizeof...(Args)>{});
			}
		};


		template <typename Func, typename... Args>
		decltype(auto) match(Func&& func, Args&&... args) {

			visitor_invocability_checker<Func, type_list, Args...>::check_all();
			visit_invoker<variant, Func, Args...> Visitor(this, std::forward<Func>(func), std::forward<Args>(args)...);
			return Visitor.visit();
		}

		template <typename Func, typename... Args>
		decltype(auto) match(Func&& func, Args&&... args) const {
			visitor_invocability_checker<Func, type_list, Args...>::check_all();
			visit_invoker<const variant, Func, Args...> Visitor(this, std::forward<Func>(func), std::forward<Args>(args)...);
			return Visitor.visit();
		}
	private:

		template <typename VariantType>
		void copy_from(const VariantType& other) {
			clear();
			index = other.index;
			(other).match([this](auto&& value) {
				using T = std::decay_t<decltype(value)>;
				storage.template construct<T>(std::forward<decltype(value)>(value));
				});
		}
		template <typename VariantType>
		void move_from(VariantType&& other) {
			clear();
			index = other.index;
			other.match([this](auto& value) {
				using T = std::decay_t<decltype(value)>;
				storage.template construct<T>(std::move(value));
				});
		}
		void clear() {
			match([this](auto& value) {
				using T = std::decay_t<decltype(value)>;
				storage.template clear<T>();
				});


		}
		erasure::erased<types...> storage;
		char index;

	};
	template <typename ...Ty>
	struct visitor :Ty...{
		using Ty::operator()...;
		explicit visitor(Ty&&... fns) :Ty(std::forward<Ty>(fns))... {}
	};
	template<typename... Ts>
	constexpr auto make_visitor(Ts&&... ts) {
		return visitor<std::decay_t<Ts>...>(std::forward<Ts>(ts)...);
	}
	template <typename... Ts>
	visitor(Ts&&...) -> visitor<std::decay_t<Ts>...>;
}