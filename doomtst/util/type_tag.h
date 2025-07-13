#pragma once
#include <type_traits>

namespace type_tag {

    // Check if T has nested alias Tag
    template<typename T, typename Tag>
    constexpr bool has_any_tag = requires { typename T::Tag; };

    // Check if T has nested alias Tag and that alias == Expected
    template<typename T, typename Tag, typename Expected>
    constexpr bool has_tag = requires {
        typename T::Tag;
    }&& std::is_same_v<typename T::Tag, Expected>;

}