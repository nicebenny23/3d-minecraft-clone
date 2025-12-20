#include <format>
#include "exception.h"
#include <cstdint>
#include <type_traits>
#pragma once
namespace stn {

    template<typename T>
    struct typed_count {
        uint32_t count;

        constexpr typed_count() : count(0) {}
        template<typename Base>
            requires std::is_base_of_v<Base, T>
        constexpr operator typed_count<Base>() const noexcept {
            return typed_count<Base>(count);
        }
        constexpr explicit typed_count(uint32_t cnt) : count(cnt) {}

        explicit constexpr operator bool() const noexcept {
            return count != 0;
        }

        constexpr auto operator<=>(const typed_count&) const noexcept = default;
        constexpr explicit operator uint32_t() const {
            return count;
        }
        constexpr explicit operator size_t() const {
            return static_cast<size_t>(count);
        }

        constexpr typed_count& operator++() {
            ++count;
            return *this;
        }
        constexpr typed_count operator++(int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        constexpr typed_count& operator--() {
            if (count == 0)
                stn::throw_range_exception("typed_count<{}>: decrement below zero", typeid(T).name());
            --count;
            return *this;
        }
        constexpr typed_count operator--(int) {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        constexpr typed_count operator+(uint32_t n) const {
            return typed_count(count + n);
        }
        constexpr typed_count operator+(typed_count other) const {
            return typed_count(count + other.count);
        }
        constexpr typed_count& operator+=(uint32_t n) {
            count += n;
            return *this;
        }
        constexpr typed_count& operator+=(typed_count other) {
            count += other.count;
            return *this;
        }

        constexpr typed_count operator-(uint32_t n) const {
            if (n > count) {
                stn::throw_range_exception("typed_count<{}>:cannot subtract {} from {}", typeid(T).name(), n, count);
            }
            return typed_count(count - n);
        }
        constexpr typed_count operator-(typed_count other) const {
            if (other.count > count) {
                stn::throw_range_exception("typed_count<{}>: cannot subtract {} from {}", typeid(T).name(), other.count, count);
            }
            return typed_count(count - other.count);
        }
        constexpr typed_count& operator-=(uint32_t n) {
            if (n > count) {
                stn::throw_range_exception("typed_count<{}>: cannot subtract {} from {}", typeid(T).name(), n, count);
            }
            count -= n;
            return *this;
        }
        constexpr typed_count& operator-=(typed_count other) {
            if (other.count > count) {
                stn::throw_range_exception("typed_count<{}>: cannot subtract {} from {}", typeid(T).name(), other.count, count);
            }
            count -= other.count;
            return *this;
        }
    };

    template<typename T>
    struct typed_index {
		using count_type = typed_count<T>;
        uint32_t index;
        //tag inheritance operator
        template<typename Base>
            requires std::is_base_of_v<Base, T>
        constexpr operator typed_index<Base>() const noexcept {
            return typed_index<Base>(index);
        }
        constexpr explicit typed_index(uint32_t ind) : index(ind) {}
        constexpr auto operator<=>(const typed_index&) const noexcept = default;
        constexpr explicit operator uint32_t() const { return index; }
        constexpr explicit operator size_t() const { return static_cast<size_t>(index); }

        constexpr typed_index& operator++() noexcept { ++index; return *this; }
        constexpr typed_index operator++(int) noexcept { auto tmp = *this; ++(*this); return tmp; }

        constexpr typed_index& operator--() {
			if (index==0) {
				stn::throw_range_exception("typed_index<{}>: decrement below zero", typeid(T).name());
			}
            --index;
            return *this;
        }
        constexpr typed_index operator--(int) {
            auto tmp = *this;
            --(*this);
            return tmp;
        }
        constexpr typed_index operator+(typed_count<T> cnt) const {
            return typed_index(index + cnt.count);
        };
        //deleted because count+index makes no sense only index+count does
        constexpr typed_index& operator+=(typed_count<T> cnt) noexcept {
            index += cnt.count;
            return *this;
        }
        
        constexpr count_type operator-(const typed_index& other) const noexcept {
            if (index<other.index) {
                stn::throw_range_exception("typed_index<{}>: cannot subtract index {} from {}", typeid(T).name(), other.index, index);
            }
            return count_type(index - other.index);
        }
        constexpr typed_index operator-(count_type cnt) const noexcept {
            if (index<cnt.count) {
                stn::throw_range_exception("typed_index<{}>: cannot subtract count {} from {}", typeid(T).name(), cnt.count, index);
            }
            return typed_index(index - cnt.count);
        }
        constexpr typed_index& operator-=(const count_type& cnt) noexcept {
            if (index<cnt.count ) {
                stn::throw_range_exception("typed_index<{}>: cannot subtract count {} from {}", typeid(T).name(), cnt.count, index);
            }
            index -= cnt.count;
            return *this;
        }

        typed_index() = delete;
    }; 
    struct default_tag {};
    using index_t = stn::typed_index<default_tag>;
    using count_t = stn::typed_count<default_tag>;
}
#include <functional>
namespace std {
    template<typename T>
    struct hash<stn::typed_index<T>> {
        size_t operator()(stn::typed_index<T> t) const noexcept {
            return std::hash<uint32_t>{}(t.index);
        }
    };
    template<typename T>
    struct hash<stn::typed_count<T>> {
        size_t operator()(stn::typed_count<T> t) const noexcept {
            return std::hash<uint32_t>{}(t.count);
        }
    };
	template<typename T, typename CharT>
	struct formatter<stn::typed_index<T>, CharT> {
		constexpr auto parse(std::basic_format_parse_context<CharT>& ctx) {
			return ctx.begin();
		}

		template<typename FormatContext>
		auto format(const stn::typed_index<T>& ti, FormatContext& ctx) const{
			return std::format_to(ctx.out(), "{}", ti.index);
		}
	};

	template<typename T, typename CharT>
	struct formatter<stn::typed_count<T>, CharT> {
		constexpr auto parse(std::basic_format_parse_context<CharT>& ctx) {
			return ctx.begin();
		}

		template<typename FormatContext>
		auto format(const stn::typed_count<T>& tc, FormatContext& ctx) const {
			return std::format_to(ctx.out(), "{}", tc.count);
		}
	};
}
