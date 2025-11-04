#pragma once
#include <type_traits>
#include "layout.h"
#include "typelist.h"
namespace stn {
    namespace erasure {

        //type erased storage
        template<typename ...types>
        struct erased
        {
            using type_list = typename TypeList::TypeList<types...>;
            static constexpr stn::memory::layout layout = stn::memory::overlay_of<types...>;
            static constexpr size_t count() {
                return TypeList::length_v<type_list>;
            }
            template<typename T>
            static constexpr bool is_member = TypeList::contains_v<type_list, T>;

            
            template<typename T>
                requires(is_member<T>)
           inline T& get() {
                return *get_ptr<T>();
            }
            template<typename T>
                requires(is_member<T>)
            inline const T& get()  const {
                return *get_ptr<T>();
            }
            template<typename T>
                requires(is_member<T>&&std::is_copy_assignable_v<T>)
            inline void set(const T& val) {
                get<T>() = val;
            }

            template<typename T>
                requires(is_member<T>&&std::is_move_assignable_v<T>)
            inline void set(T&& val) {
                get<T>() = std::move(val);
            }

            template<typename T, typename... Args>
                requires(is_member<T>&& std::is_constructible_v<T, Args...> && !std::is_reference_v<T>)
            inline void construct(Args&&... args) {
                new (&storage) T(std::forward<Args>(args)...);
            }

            // Specialization for reference types
            template<typename T>
                requires (is_member<T>&& std::is_reference_v<T>)
            inline void construct(T ref) {
                using raw_T = std::remove_reference_t<T>;
                auto ptr = reinterpret_cast<raw_T**>(&storage);
                *ptr = &ref; // Store the address of the referent
            }

            template<typename T>
                requires(is_member<T>)
           inline void clear() {
                if constexpr (!std::is_trivially_destructible_v<T> && !std::is_reference_v<T>) {
                    get_ptr<T>()->~T();
                }
                if constexpr (std::is_reference_v<T>)
                {
                    get_ptr<T>() = nullptr;
                }
            }

        private:
            alignas(layout.alignment) unsigned char storage[layout.size];
            template<typename T>
           inline std::remove_reference_t<T>* get_ptr() {
                if constexpr (std::is_reference_v<T>)
                {
                    using raw_T = std::remove_reference_t<T>;
                    auto ptr = reinterpret_cast<raw_T**>(&storage);
                    return *ptr;
                }
                else {
                    auto p = reinterpret_cast<T*>(storage);
                    return std::launder(p);
                }
            }

            template<typename T>
            const std::remove_reference_t<T>* get_ptr() const {
                if constexpr (std::is_reference_v<T>)
                {
                    using raw_T = std::remove_reference_t<T>;
                    auto ptr = reinterpret_cast<raw_T* const*>(&storage);
                    return *ptr;
                }
                else {
                    auto p = reinterpret_cast<const T*>(storage);
                    return std::launder(p);
                }
            }
        };
    }
}