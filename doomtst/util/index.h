
#pragma once
namespace stn::typestate {

    template<typename T>
    struct typed_index {
        uint32_t index;

        constexpr explicit typed_index(uint32_t ind) : index(ind) {}
        constexpr bool operator==(const typed_index&) const noexcept = default;
        constexpr auto operator<=>(const typed_index&) const noexcept = default;
    
        // Increment / Decrement
        constexpr typed_index& operator++() noexcept { ++index; return *this; }
        constexpr typed_index operator++(int) noexcept { auto tmp = *this; ++(*this); return tmp; }

        constexpr typed_index& operator--() {
            if (index == 0) {
                throw std::out_of_range("typed_index underflow in operator--");
            };
            --index;
            return *this;
        }
        constexpr typed_index operator--(int) {
            auto tmp = *this;
            --(*this);
            return tmp;
        }
        typed_index() = delete; // no default constructor
    };

}
#include <functional>
namespace std {
	template<typename T>
	struct hash<stn::typestate::typed_index<T>> {
		size_t operator()(stn::typestate::typed_index<T> t) const noexcept {
			return std::hash<uint32_t>{}(t.index);
		}
	};
}