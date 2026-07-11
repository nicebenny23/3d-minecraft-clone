#pragma once
#include <utility>
#include <string>
#include <type_traits>
#include <stdexcept>
#include <concepts>
#include "concepts.h"
#include "exception.h"
#include "traits.h"
namespace stn {


	struct NoneType {
		constexpr explicit NoneType() {
		};
	};
	static constexpr NoneType None{ NoneType{} };

	template<typename T>
	struct Option;

	template<typename T>
	concept OptionType = stn::TypeInstantiationOf<T, Option>;


	template<typename T>
	struct Option {
		using storage_type = std::conditional_t<std::is_lvalue_reference_v<T>, std::remove_reference_t<T>*, T>;
		static_assert((!std::is_void_v<T> && !std::is_rvalue_reference_v<T>), "an option may not be void");
		union {
			NoneType none;
			storage_type elem;
		};
		using value_type = T;
		__forceinline inline 
		auto unwrap_unchecked(this auto&& self) -> stn::forward_as_member_t<decltype(self), T> {
			if constexpr (is_refrence) {
				return *std::forward<decltype(self)>(self).elem;
			}
			else {
				return std::forward<decltype(self)>(self).elem;
			}
		}

		Option() noexcept : has_value(false) {
			new (&none) NoneType();
		}
		Option(Option::NoneType) noexcept : has_value(false) {
			new (&none) NoneType();
		}
		~Option() {
			//bc compiler is often confused
			if constexpr (!std::is_trivially_destructible_v<T>) {
				if (has_value) {
					destruct();
				}
			}
		}
		static constexpr bool is_refrence = std::is_lvalue_reference_v<T>;
		template<typename U = T>
			requires (std::constructible_from<T, U&&>)
		Option(U&& val) : has_value(true) {
			construct(std::forward<U>(val));
		}

		template<typename U> requires stn::OptionType<U>&& std::constructible_from<storage_type, typename U::storage_type&&>
		Option(U&& other)
			: has_value(other.has_value) {
			if (has_value) {
				construct_pure(std::forward<U>(other).elem);
			}
			else {
				new (&none) NoneType();
			}
		}

		Option(Option&& other)requires(std::move_constructible<T>) {
			has_value = other.has_value;
			if (has_value) {
				construct_pure(std::move(other.elem));
			}
			else {
				new (&none) NoneType();
			}
		}
		Option(const Option& other) requires(std::copy_constructible<T>) {
			has_value = other.has_value;
			if (has_value) {
				construct_pure(other.elem);
			}
			else {
				new (&none) NoneType();
			}
		}
		template<typename U>
		Option& operator=(U&& other) requires(std::constructible_from<T, U&&>) {
			if (has_value) {
				destruct();
			}
			has_value = true;
			construct(std::forward<U>(other));
			return *this;
		}
		auto as_ref(this auto&& self) -> Option<apply_const_like_t<decltype(self), T>&>
			requires std::is_lvalue_reference_v<decltype(self)> {

			using inner_type = apply_const_like_t<decltype(self), T>&;
			if (self.has_value) {
				return Option<inner_type>(std::forward<decltype(self)>(self).unwrap_unchecked());
			}
			else {
				return Option<inner_type>(stn::None);
			}
		}
		auto strip_cv_ref(this auto&& self) -> Option<std::remove_cvref_t<T>>
		{

			using inner_type = std::remove_cvref_t<T>;
			if (self.has_value) {
				return Option<inner_type>(std::forward<decltype(self)>(self).unwrap_unchecked());
			}
			else {
				return Option<inner_type>(stn::None);
			}
		}
		Option<std::remove_cvref_t<T>> const copied() requires std::is_reference_v<T>&& std::is_copy_constructible_v<std::remove_cvref_t<T>> {
			if (has_value) {
				return Option<std::remove_cvref_t<T>>(unwrap_unchecked());
			}
			else {
				return stn::None;
			}
		}
		[[nodiscard]] T flatten() const requires OptionType<T> {
			if (!has_value) {
				return T(None);
			}
			return unwrap_unchecked();
		}
		Option& operator=(const Option& other)
			requires std::constructible_from<storage_type, const storage_type&> {
			if (this != &other) {
				assign_from(other);
			}
			return *this;
		}
		Option& operator=(Option&& other)
			requires std::constructible_from<storage_type, storage_type&&> {
			if (this != &other) {
				assign_from(std::move(other));
			}
			return *this;
		}
		using refrence_type = Option<T&>;

		using const_refrence_type = Option<T&>;
		template<OptionType Opt>
		Option& operator=(Opt&& other) requires std::constructible_from<storage_type, typename Opt::storage_type&&> {
			assign_from(std::forward<Opt>(other));
			return *this;
		}
		Option& operator=(NoneType) {
			clear();
			return *this;
		}

		void clear() {
			if (has_value) {
				destruct();
				has_value = false;
			}
		}

		template<typename... Args>
			requires (!std::is_reference_v<T>)
		void emplace(Args&&... args) {
			clear();
			construct(std::forward<Args>(args)...);
			has_value = true;
		}



		bool operator==(const Option& other) const requires std::equality_comparable<T> {
			if (has_value && other.has_value) {
				return unwrap_unchecked() == other.unwrap_unchecked();
			}
			return false;
		}

		bool operator!=(const Option& other) const requires std::equality_comparable<T> {
			return !(*this == other);
		}
		template<typename U>
			requires std::equality_comparable_with<T, U>
		bool operator==(const U& other) const {
			return has_value && unwrap_unchecked() == other;
		}
		template<typename U>
		bool operator!=(const U& other) const {
			return !(*this == other);
		}
		bool operator==(NoneType) const {
			return is_none();
		}

		bool operator!=(NoneType) const {
			return is_some();
		}

		__forceinline inline bool is_some() const noexcept {
			return has_value;
		}

		template<std::predicate<const T&> Pred>
		bool is_some_and(Pred&& pred) const {
			if (has_value) {
				return std::invoke(std::forward<Pred>(pred), unwrap_unchecked());
			}
			return false;
		}
		__forceinline inline bool is_none() const noexcept {
			return !has_value;
		}
		template<std::predicate<const T&> Pred>
		bool is_none_or(Pred&& pred) const {
			if (has_value) {
				return std::invoke(std::forward<Pred>(pred), unwrap_unchecked());
			}
			return true;
		}

		explicit operator bool() const noexcept {
			return has_value;
		}

		auto expect(this auto&& self, const char* msg) -> stn::forward_as_member_t<decltype(self), T> {
			if (self.has_value) {
				return std::forward<decltype(self)>(self).unwrap_unchecked();
			}
			else {
				throw std::logic_error(msg);
			}
		}

		auto unwrap(this auto&& self) -> stn::forward_as_member_t<decltype(self), T> {
			if (self.has_value) {
				return std::forward<decltype(self)>(self).unwrap_unchecked();
			}
			else {
				throw std::logic_error("Expected Option to have a value");
			}
		}
		template<typename U>
		T unwrap_or(U&& default_val) const requires std::constructible_from<T, U&&> {
			return has_value ? unwrap_unchecked() : default_val;
		}

		T unwrap_or_default() const requires std::constructible_from<T> {
			return has_value ? T(unwrap_unchecked()) : T();
		}
		template<typename DefaultFunc>
		auto unwrap_or_else(this auto&& self, DefaultFunc&& default_func) -> stn::forward_as_member_t<decltype(self), T>
			requires std::invocable<DefaultFunc>&& std::same_as<std::invoke_result_t<DefaultFunc>, T> {
			if (self.has_value) {
				return std::forward<decltype(self)>(self).unwrap_unchecked();
			}
			return std::invoke(std::forward<DefaultFunc>(default_func));
		}



		template<typename Some_Func, typename None_Func>
		auto match(Some_Func&& some_function, None_Func&& none_function) const requires std::invocable<Some_Func, T>&& std::invocable<None_Func> {
			if (has_value) {
				return std::invoke(std::forward<Some_Func>(some_function), elem);
			}
			return std::invoke(std::forward<None_Func>(none_function));

		}
		template<std::predicate<const T&> Pred>
		auto filter(this auto&& self, Pred&& pred) -> std::remove_reference_t<decltype(self)> {
			using self_type = std::remove_reference_t<decltype(self)>;
			if (std::forward<self_type>(self).is_some_and(std::forward<Pred>(pred))) {
				return std::forward<self_type>(self);
			}
			return stn::None;
		}

		Option<T> retain(this auto&& self, bool should_keep) {
			if (should_keep && self.has_value) {
				return Option<T>(std::forward<decltype(self)>(self).unwrap_unchecked());
			}
			return stn::None;
		}

		template<typename U>
		T& fallback(U&& value) requires std::constructible_from<T, U&&> {
			if (!has_value) {
				construct(std::forward<U>(value));
			}
			return unwrap_unchecked();
		}
		template<typename DefaultFunc>
		T& fallback_on(DefaultFunc&& default_func)
			requires stn::invocable_to<T,DefaultFunc> {
			if (!has_value) {
				has_value = true;
				construct(std::invoke(std::forward<DefaultFunc>(default_func)));
			}
			return unwrap_unchecked();
		}

		template<typename Func>
		auto map(this auto&& self, Func&& f) -> Option< std::invoke_result_t<Func, stn::forward_as_member_t<decltype(self), T>>> requires stn::nonvoid_invokable<Func, stn::forward_as_member_t<decltype(self), T>> {
			using U = std::invoke_result_t<Func, stn::forward_as_member_t<decltype(self), T>>;
			if (self.has_value) {
				return std::invoke(std::forward<Func>(f), std::forward<decltype(self)>(self).unwrap_unchecked());
			}
			return stn::None;
		}

		template<typename C, typename R> requires stn::decays_to<T, C>
		stn::Option <R> map_member(R(C::* member)() const) const {
			return map(member);
		}
		template<typename C, typename R> requires stn::decays_to<T, C>
		stn::Option<R> map_member(R(C::* member)())& {
			return map(member);
		}
		template<typename C, typename R> requires stn::decays_to<T, C>
		stn::Option<R> map_member(R(C::* member)())&& {
			return map(member);
		}
		template<typename R, typename C> requires stn::decays_to<T, C>
		stn::Option <const R&> member_ref(R C::* member) const& {
			return map(member);
		}
		template<typename R, typename C>
		stn::Option<R&> member_ref(R C::* member)& requires stn::decays_to<T, C> {
			return map(member);
		}
		template<typename R, typename C>
		stn::Option<R> member(R C::* member) const& requires stn::decays_to<T, C> {
			return map(member).copied();
		}
		template<typename R, typename C>
		stn::Option<R> member(R C::* member)& requires stn::decays_to<T, C> {
			return map(member).copied();
		}
		template<typename R, typename C>
		stn::Option<R> member(R C::* member) && requires stn::decays_to<T, C> {
			auto ref = map(member);

			return ref.copied();
		}
		template<typename Func>
		auto then(this auto&& self, Func&& f) -> decltype(self)
			requires stn::void_invokable<Func, stn::forward_as_member_t<decltype(self), T>> {
			if (self.has_value) {
				std::invoke(std::forward<Func>(f), std::forward<decltype(self)>(self).unwrap_unchecked());
			}
			return std::forward<decltype(self)>(self);
		}

		template<typename Func>
		auto and_then(this auto&& self, Func&& f) -> std::invoke_result_t<Func, stn::forward_as_member_t<decltype(self), T>>
			requires stn::nonvoid_invokable<Func, stn::forward_as_member_t<decltype(self), T>>&& OptionType<std::invoke_result_t<Func, stn::forward_as_member_t<decltype(self), T>>> {
			if (self.has_value) {
				return std::invoke(std::forward<Func>(f), std::forward<decltype(self)>(self).unwrap_unchecked());
			}
			return stn::None;
		}
	private:

		//constructs while not missing
		template<typename ...Args> requires std::constructible_from<storage_type, Args&&...>
		inline void construct_pure(Args&&... args) {
			new (&elem) storage_type(std::forward<Args>(args)...);
		}
		template<typename ...Args>
		inline void construct(Args&&... args)
			requires std::constructible_from<T, Args&&...> {
			if constexpr (is_refrence) {
				new (&elem) storage_type(&std::forward<Args>(args)...);
			}
			else {
				new (&elem) T(std::forward<Args>(args)...);
			}
		}

		inline void destruct() {
			if constexpr (!is_refrence) {
				elem.~T();
			}
			new (&none) NoneType();
		}
		template<typename Other> requires std::constructible_from<storage_type, typename std::remove_cvref_t<Other>::storage_type&&>
		void assign_from(Other&& other) {
			using other_storage = typename std::remove_cvref_t<Other>::storage_type&&;
			if (other.has_value) {
				if (has_value) {
					if constexpr (std::assignable_from<storage_type, other_storage>) {
						elem = std::move(other.elem);
						return;
					}
					else {
						destruct();
						construct_pure(std::forward<Other>(other).elem);
					}
				}
				else {
					construct_pure(std::forward<Other>(other).elem);
					has_value = true;
				}
			}
			else {
				clear();
			}
		}
		template<typename U>
		friend struct Option;
		bool has_value;
	};
	template<typename T>
	using OptRef = stn::Option<T&>;
	template<typename T, typename... Args>
	Option<T> MakeOption(Args&&... args) {
		return Option<T>(T(std::forward<Args>(args)...));
	}

	template<typename T>
	std::partial_ordering partial_compare(const Option<T>& lhs, const Option<T>& rhs)
		requires requires(const T& a, const T& b) {
			{
				a <=> b
			} -> std::convertible_to<std::partial_ordering>;
	}
	{
		if (!lhs.is_some() && !rhs.is_some()) return std::partial_ordering::equivalent;
		if (!lhs.is_some()) return std::partial_ordering::less;
		if (!rhs.is_some()) return std::partial_ordering::greater;
		return lhs.unwrap() <=> rhs.unwrap();
	}
	template<typename T>
	bool operator<(const Option<T>& lhs, const Option<T>& rhs) {
		return partial_compare(lhs, rhs) == std::partial_ordering::less;
	}

	template<typename T>
	bool operator>(const Option<T>& lhs, const Option<T>& rhs) {
		return partial_compare(lhs, rhs) == std::partial_ordering::greater;
	}

	template<typename T>
	bool operator<=(const Option<T>& lhs, const Option<T>& rhs) {
		auto cmp = partial_compare(lhs, rhs);
		return cmp == std::partial_ordering::less || cmp == std::partial_ordering::equivalent;
	}

	template<typename T>
	bool operator>=(const Option<T>& lhs, const Option<T>& rhs) {
		auto cmp = partial_compare(lhs, rhs);
		return cmp == std::partial_ordering::greater || cmp == std::partial_ordering::equivalent;
	}
	template<typename T>
	concept PartiallyComparable = requires(T a, T b) {
		{
			a <=> b
		} -> std::convertible_to<std::partial_ordering>;
	};

	template<typename T, typename Func> requires
		std::invocable<Func&&, const T&>&&
		PartiallyComparable<std::invoke_result_t<Func&&, const T&>>
		stn::Option<T> min_some_on_map(const stn::Option<T>& first, const stn::Option<T>& second, Func&& fn) {
		if (first.is_none()) {
			return second;
		}
		if (second.is_none()) {
			return first;
		}
		decltype(auto) first_key = std::invoke(std::forward<Func>(fn), first.unwrap_unchecked());
		decltype(auto) second_key = std::invoke(std::forward<Func>(fn), second.unwrap_unchecked());
		if (first_key < second_key) {
			return first;
		}
		return second;
	}

}
#include <format>
namespace std {

	template<typename T, typename CharT>
	struct formatter<stn::Option<T>, CharT> : formatter<T, CharT> {
		template<typename FormatContext> requires std::formattable<T, CharT>
		auto format(const stn::Option<T>& opt, FormatContext& ctx) const {
			return opt.match(
				[&](const T& value) {
					auto out = std::format_to(ctx.out(), "Some(");
					out = formatter<T, CharT>::format(opt.unwrap(), ctx);
					return std::format_to(out, ")");
				},
				[&]() { return std::format_to(ctx.out(), "None"); }
			);
		}
	};

}