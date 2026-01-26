#pragma once
#include <utility>
#include <string>
#include <type_traits>
#include <stdexcept>
#include <concepts>
#include "concepts.h"
#include "erased.h"
#include "traits.h"
namespace stn {


	struct NoneType {
		struct NullTag {
		};
		constexpr explicit NoneType(NullTag) {
		};
	};
	static constexpr NoneType None{ NoneType::NullTag{} };

	template<typename T>
	struct Option;

	template<typename>
	struct is_option_t : std::false_type {
	};

	template<typename T>
	struct is_option_t<Option<T>> : std::true_type {
	};
	template<typename T>
	concept OptionType =
		is_option_t<std::remove_cvref_t<T>>::value;


	template<typename T>
	struct Option {
		using value_type = T;
		Option() noexcept : has_value(false), value() {
		}
		Option(Option::NoneType) noexcept : has_value(false), value() {
		}
		~Option() {
			//bc compiler is often confused
			if constexpr (!std::is_trivially_destructible_v<T>) {
				clear();
			}
		}

		template<typename U = T>
			requires (!std::is_reference_v<T>&& std::constructible_from<T, U&&>)
		Option(U&& val) : has_value(true), value() {
			static_assert(sizeof(U) > 0, "Option<T>: T must be a complete type");
			value.construct<T>(std::forward<U>(val));
		}

		Option(T& val) requires (std::is_reference_v <T>) : has_value(true), value() {
			value.construct<T>(val);
		}

		Option(const Option& other) requires std::copy_constructible<T>
			: has_value(other.has_value), value() {
			if (has_value) {
				value.construct<T>(other.value.get<T>());
			}
		}


		Option(Option&& other) noexcept(std::is_nothrow_move_constructible_v<T>) requires (!std::is_reference_v<T>)
			: has_value(other.has_value), value() {
			if (has_value) {
				value.construct<T>(std::move(other.value.get<T>()));
				other.has_value = false;

			}
		}

		Option& operator=(const Option& other) requires std::copy_constructible<T> {
			if (this != &other) {
				//non_null_reset
				if (has_value) {
					value.clear<T>();
				}
				has_value = other.has_value;
				if (has_value) {
					value.construct<T>(other.value.get<T>());
				}
			}
			return *this;
		}

		Option<T&> as_ref()& {
			if (has_value) {
				return Option<T&>{value.get<T>()};
			}
			else {
				return stn::None;
			}
		}

		Option<const T&> as_ref() const& {
			if (has_value) {
				return Option<const T&>{value.get<T>()};
			}
			else {
				return stn::None;
			}
		}
		Option<std::remove_cvref_t<T>> const copied() requires std::is_reference_v<T>&& std::is_copy_constructible_v<std::remove_cvref_t<T>> {
			if (has_value) {
				return Option<std::remove_cvref_t<T>>(value.get<T>());
			}
			else {
				return stn::None;
			}
		}
		[[nodiscard]] T flatten() const requires OptionType<T> {
			if (!has_value) {
				return T(None);
			}

			return value.get<T>();

		}

		Option& operator=(Option&& other) noexcept(std::is_nothrow_move_assignable_v<T>) {
			if (this != &other) {
				//non_null_reset
				if (has_value) {
					value.clear<T>();
				}
				has_value = other.has_value;
				if (has_value) {
					if constexpr (std::is_reference_v<T>) {
						value.construct<T>(other.value.get<T>()); // just copy the pointer 
					}
					else {
						value.construct<T>(std::move(other.value.get<T>())); // normal move
					}
					other.has_value = false;
				}
			}
			return *this;
		}

		Option& operator=(NoneType) {
			//good
			clear();
			return *this;
		}

		void clear() {
			if (has_value) {
				value.clear<T>();
				has_value = false;
			}
		}

		template<typename... Args>
			requires (!std::is_reference_v<T>)
		void emplace(Args&&... args) {
			clear();
			value.construct<T>(std::forward<Args>(args)...);
			has_value = true;
		}



		bool operator==(const Option& other) const requires std::equality_comparable<T> {
			if (has_value && other.has_value) {
				return value.get<T>() == other.value.get<T>();
			}
			return false;
		}

		bool operator!=(const Option& other) const requires std::equality_comparable<T> {
			return !(*this == other);
		}
		template<typename U>
			requires std::equality_comparable_with<T, U>
		bool operator==(const U& other) const {
			return has_value && value.get<T>() == other;
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

		bool is_some() const noexcept {
			return has_value;
		}

		template<std::predicate<const T&> Pred>
		bool is_some_and(Pred&& pred) const {
			if (has_value) {
				return std::invoke(std::forward<Pred>(pred), value.get<T>());
			}
			return false;
		}
		bool is_none() const noexcept {
			return !has_value;
		}
		template<std::predicate<const T&> Pred>
		bool is_none_or(Pred&& pred) const {
			if (has_value) {
				return std::invoke(std::forward<Pred>(pred), value.get<T>());
			}
			return true;
		}


		explicit operator bool() const noexcept {
			return has_value;
		}
		T& expect(const char* Msg = "Expected Option to have a value") {
			if (!has_value) {
				throw std::logic_error(std::string("Option error: ") + Msg);
			}
			return value.get<T>();
		}
		const T& expect(const char* Msg = "Expected Option to have a value") const {
			if (!has_value) {
				throw std::logic_error(std::string("Option error: ") + Msg);
			}
			return value.get<T>();
		}

		T& unwrap_unchecked() {
			return value.get<T>();
		}

		const T& unwrap_unchecked() const {
			return value.get<T>();
		}

		T& unwrap() {
			return expect("Called unwrap on None");
		}
		//throws if the Option is none
		const T& unwrap() const {
			return expect("Called unwrap on None");
		}
		T unwrap_or(const T& default_val) const noexcept(std::is_nothrow_copy_constructible_v<T>) {
			return has_value ? value.get<T>() : default_val;

		}
		template<typename DefaultFunc>
		auto unwrap_or_else(DefaultFunc&& default_func) -> T
			requires std::invocable<DefaultFunc>&& std::same_as<std::invoke_result_t<DefaultFunc>, T> {
			if (has_value) {
				return value.get<T>();
			}
			return std::invoke(std::forward<DefaultFunc>(default_func));
		}



		template<typename Some_Func, typename None_Func>
		auto match(Some_Func&& some_function, None_Func&& none_function) const requires std::invocable<Some_Func, T>&& std::invocable<None_Func> {
			if (has_value) {
				return std::invoke(std::forward<Some_Func>(some_function), value.get<T>());
			}
			return std::invoke(std::forward<None_Func>(none_function));

		}
		template<std::predicate<const T&> Pred>
		auto filter(Pred&& pred) const -> Option<T> {
			if (is_some_and(std::forward<Pred>(pred))) {
				return Option<T>(value.get<T>());
			}
			return stn::None;
		}
		template<std::predicate<T> Pred>
		auto filter(Pred&& pred) && -> Option<T> {
			if (is_some_and(std::forward<Pred>(pred))) {
				has_value = false;
				return Option<T>(std::move(value.get<T>()));
			}
			return stn::None;
		}

		Option<T> retain(this auto&& self, bool should_keep) {
			if (should_keep && self.has_value) {
				return Option<T>(std::forward<decltype(self)>(self).value.get<T>());
			}
			return stn::None;
		}
		template<typename Func>
		auto map(Func&& f) const -> Option< std::invoke_result_t<Func, const T&>> requires std::invocable<Func, const T&> && !std::is_void_v<std::invoke_result_t<Func, const T&>>{
			using U = std::invoke_result_t<Func, const T&>;
			if (has_value) {
				return Option<U>(std::invoke(std::forward<Func>(f), value.get<T>()));
			}
			return stn::None;
		}
		template<typename Func>
		auto map(Func&& f) & ->Option< std::invoke_result_t<Func, T&>> requires std::invocable<Func, T&> && !std::is_void_v<std::invoke_result_t<Func, T&>>{
			using U = std::invoke_result_t<Func, T&>;
			if (has_value) {
				return Option<U>(std::invoke(std::forward<Func>(f), value.get<T>()));
			}
			return stn::None;
		}
		template<typename Func>
		auto map(Func&& f) && ->Option< std::invoke_result_t<Func, T>> requires std::invocable<Func, T> && !std::is_void_v<std::invoke_result_t<Func, T>>{
			using U = std::invoke_result_t<Func, T>;
			if (has_value) {
				has_value = false;
				return Option<U>(stn::invoke_carry(std::forward<Func>(f), value.get<T>()));
			}
			return stn::None;

		}
	
		template<typename C, typename R> requires stn::decays_to<T, C>
		stn::Option <R> map_member(R(C::* member)() const) const{
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
		template<typename R, typename C> requires stn::decays_to<T,C>
		stn::Option <const R&> member(R C::*member) const{
			return map(member);
		}
		template<typename R, typename C>
		stn::Option<R&> member(R C::* member) & requires stn::decays_to<T, C> {
			return map(member);
		}
		template<typename R, typename C>
		stn::Option<R> member(R C::* member)&& requires stn::decays_to<T, C> {
			return map(member);
		}
		template<typename Func>
		Option& then(Func&& f)&
			requires std::invocable<Func, T&>&& std::is_void_v<std::invoke_result_t<Func, T&>> {
			if (has_value) {
				std::invoke(std::forward<Func>(f), value.get<T>());
			}
			return *this; // return lvalue
		}

		template<typename Func>
		Option then(Func&& f) && requires std::invocable<Func, T>&& std::is_void_v<std::invoke_result_t<Func, T&&>> {
			if (has_value) {
				std::invoke(std::forward<Func>(f), std::forward<T>(value.get<T>()));
			}
			return std::move(*this);
		}

		template<typename Func> requires std::invocable<Func, const T&>&& OptionType<std::invoke_result_t<Func, const T&>>
		auto and_then(Func&& f) const -> std::invoke_result_t<Func, const T&> {
			if (has_value) {
				return std::invoke(std::forward<Func>(f), value.get<T>());
			}
			return stn::None;
		}
		template<typename Func> requires std::invocable<Func, T>&& OptionType<std::invoke_result_t<Func, T>>
		auto and_then(Func&& f) && -> std::invoke_result_t<Func, T> {
			if (has_value) {
				has_value = false;
				return std::invoke(std::forward<Func>(f), std::move(value.get<T>()));
			}
			return stn::None;
		}
	private:
		bool has_value;
		stn::erasure::erased<T> value;
	};

	template<typename T, typename... Args>
	Option<T> Construct(Args&&... args) {
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
		return lhs.unwrap() <=> rhs.unwrap(); // defer to T’s partial ordering
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
}
#include <format>
namespace std {

	template<typename T, typename CharT>
	struct formatter<stn::Option<T>, CharT> : formatter<T, CharT> {
		template<typename FormatContext> requires std::formattable<T, CharT>
		auto format(const stn::Option<T>& opt, FormatContext& ctx) const{
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