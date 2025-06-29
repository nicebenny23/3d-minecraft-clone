
#pragma once
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <type_traits>
namespace Cont {

    template <typename T, size_t N>
        requires std::is_default_constructible_v<T>
    
    struct List {
        T elements[N]{};
        constexpr List()  = default ;
        constexpr List(std::initializer_list<T> list) noexcept {
            size_t i = 0;
            for (const T& value : list) {
                if (i < N) { // Prevent overflow
                    elements[i] = value;
                    i++;
                }
            }
        }
        constexpr void fill(const T& value) {
            for (size_t i = 0; i < N; ++i) {
                elements[i] = value;
            }
        }
        constexpr T& operator[](size_t index) {
            if (index >= N)
                throw std::out_of_range("List index out of bounds");
            return elements[index];
        }
        constexpr const T& operator[](size_t index) const {
            if (index >= N)
                
                throw std::out_of_range("List index out of bounds");
            return elements[index];
        }
        constexpr size_t length() const {
            return N;
        }
        // Data access
        constexpr T* data() { return elements; }
        constexpr const T* data() const { return elements; }
        constexpr T* begin() { return elements; }
        constexpr T* end() { return elements + N; }

        constexpr const T* begin() const { return elements; }
        constexpr const T* end() const { return elements + N; }

        constexpr  bool operator==(const List& other) const
        {
            for (size_t i = 0; i < length(); i++)
            {
                if (other.elements[i] != elements[i])
                {
                    return false;
                }
            }
            return true;
        }
        constexpr  bool operator!=(const List& other) const
        {
            return !(*this == other);
        }
        constexpr T& front() {
            return elements[0];
        }
        constexpr const T& front() const {
            return elements[0];
        }
        constexpr T& back() {
            return elements[N - 1];
        }
        constexpr const T& back() const {
            return elements[N - 1];
        }
    };

}
