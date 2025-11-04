#include <format>
#pragma once
namespace stn {

    template<typename T>
    struct typed_index {
        uint32_t index;

        constexpr explicit typed_index(uint32_t ind) : index(ind) {}
        constexpr bool operator==(const typed_index&) const noexcept = default;
        constexpr auto operator<=>(const typed_index&) const noexcept = default;
        constexpr explicit operator uint32_t() const { return index; }
        constexpr explicit operator size_t() const { return static_cast<size_t>(index); }
        // Increment / Decrement
        constexpr typed_index& operator++() noexcept { ++index; return *this; }
        constexpr typed_index operator++(int) noexcept { auto tmp = *this; ++(*this); return tmp; }

        constexpr typed_index& operator--() {
            --index;
            return *this;
        }
        constexpr typed_index operator--(int) {
            auto tmp = *this;
            --(*this);
            return tmp;
        }
        constexpr typed_index operator+(size_t n) const noexcept {
            return typed_index(index + static_cast<uint32_t>(n));
        }

        // Addition assignment
        constexpr typed_index& operator+=(size_t n) noexcept {
            index += static_cast<uint32_t>(n);
            return *this;
        }
        typed_index() = delete; // no default constructor
    };

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
    struct formatter<stn::typed_index<T>> {
        // No fallback; we define formatting directly
        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin(); // no format specifiers supported
        }

        template<typename FormatContext>
        auto format(const stn::typed_index<T>& ti, FormatContext& ctx) {
            return std::format_to(ctx.out(), "{}", ti.index);
        }
    };

}
