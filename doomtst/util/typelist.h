#pragma once
#include <type_traits>
#include <cstddef>

namespace TypeList {

    // The TypeList container
    template<typename... Types>
    struct TypeList {};

    // Get the length of the list
    template<typename List>
    struct Length;

    template<typename... Types>
    struct Length<TypeList<Types...>> {
        static constexpr std::size_t value = sizeof...(Types);
    };

    // Check if a type T is contained in a TypeList
    template<typename List, typename T>
    struct Contains;

    template<typename T>
    struct Contains<TypeList<>, T> : std::false_type {};

    template<typename Head, typename... Tail, typename T>
    struct Contains<TypeList<Head, Tail...>, T> : std::conditional_t<
        std::is_same<Head, T>::value,
        std::true_type,
        Contains<TypeList<Tail...>, T>
    > {
    };

    // Append a type NewType to a TypeList
    template<typename List, typename NewType>
    struct Append;

    template<typename... Types, typename NewType>
    struct Append<TypeList<Types...>, NewType> {
        using type = TypeList<Types..., NewType>;
    };

    // Get the type at index I
    template<std::size_t I, typename List>
    struct TypeAt;

    template<std::size_t I, typename Head, typename... Tail>
    struct TypeAt<I, TypeList<Head, Tail...>> : TypeAt<I - 1, TypeList<Tail...>> {};

    template<typename Head, typename... Tail>
    struct TypeAt<0, TypeList<Head, Tail...>> {
        using type = Head;
    };

    template<typename List>
    struct for_each;

    template<typename List>
    struct for_each;

    template<typename... Types>
    struct for_each<TypeList<Types...>> {
        template<typename F, typename... Args>
        static void apply(Args&&... args) {
            (F::template call<Types>(std::forward<Args>(args)...), ...);
        }
    };

}
