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
            static constexpr bool is_member =TypeList::contains_v<type_list,T>;

			template<typename T>
				requires(is_member<T>)
			inline T&& get()&& {
				return std::move(*get_ptr<T>());
			}
            template<typename T>
                requires(is_member<T>)
           inline T& get()&{
                return *get_ptr<T>();
            }
            
            template<typename T>
                requires(is_member<T>)
            inline const std::remove_reference_t<T>& get()  const&{
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
                requires(is_member<T>&& std::is_reference_v<T>&& std::is_lvalue_reference_v<T>)
            inline void construct(T ref) {
                //set underlying ptr to ref;
                using raw_T = std::remove_reference_t<T>;
                *reinterpret_cast<raw_T**>(&storage)= &ref;
            
            }

            template<typename T>
                requires(is_member<T>&& !std::is_reference_v<T>)
            inline void clear() {
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    get_ptr<T>()->~T();
                }
            }

            template<typename T> 
                requires(is_member<T> && std::is_reference_v<T>)
            inline void clear() {
                    //set underlying ptr to nullptr;
                    using raw_T = std::remove_reference_t<T>;
                    *reinterpret_cast<raw_T**>(&storage) = nullptr;
            }

        private:
            alignas(layout.alignment) unsigned char storage[layout.size];
            template<typename T>
           std::remove_reference_t<T>* get_ptr() {
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