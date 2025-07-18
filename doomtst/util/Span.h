#include <iterator> // for std::distance
#include <stdexcept>
#include "mathutil.h"
template <typename T>
struct Span {
    using iterator = T*;
    using const_iterator = const T*;

    T* ptr = nullptr;
    size_t length = 0;

    constexpr Span() noexcept = default;

    constexpr Span(T* data, size_t len) noexcept
        : ptr(data), length(len) {
    }

    template <size_t N>
    constexpr Span(T(&arr)[N]) noexcept
        : ptr(arr), length(N) {
    }

    // Generic constructor for containers with begin/end
    template <typename Container>
    Span(Container& c)
        : ptr(std::data(c))
        , length(static_cast<size_t>(std::distance(std::begin(c), std::end(c))))
    {
    }

    constexpr size_t size() const noexcept { return length; }
    constexpr bool empty() const noexcept { return length == 0; }

    T& operator[](size_t i) const { 
        if (length<=i)
        {
            throw std::out_of_range("Index out of range");
        }
        return ptr[i]; 
    }
   const iterator begin() const noexcept { return ptr; }
    const iterator end() const noexcept { return ptr + length; }
    iterator begin() const noexcept { return ptr; }
    iterator end() const noexcept { return ptr + length; }
    Span operator()(size_t start, size_t end) {

        if (start > end || end > length) const{
            throw std::out_of_range("Invalid slice indices");
        };
        return Span(ptr + start, end - start);
    }
    Span subspan(size_t start, size_t count) const{
        if (start >= length) {
            throw std::out_of_range("start out of range");
        }
        size_t max_count = length - start;
        return Span(ptr + start, Min(count,max_count));
    }
    
};
