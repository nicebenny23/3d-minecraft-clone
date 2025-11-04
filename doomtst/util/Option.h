#pragma once
#include <utility>
#include <string>
#include <type_traits>
#include <stdexcept>
#include <concepts>
#include "erased.h"
namespace stn {
    

    struct NoneType {
        struct NullTag {};
        constexpr explicit NoneType(NullTag) {};
    };
    static constexpr NoneType None{ NoneType::NullTag{} };
 
    
    template<typename T>
    struct Option {
        Option() noexcept : has_value(false),value() {}
        Option(Option::NoneType) noexcept : has_value(false), value() {}

        
        template<typename U = T>
            requires (!std::is_reference_v<T>)
        Option(const U& val) : has_value(true), value() {
            static_assert(sizeof(U) > 0, "Option<T>: T must be a complete type");
            value.construct<T>(val);
        }

       
        template<typename U = T>
            requires (!std::is_reference_v<T>)
        Option(U&& val) noexcept(std::is_nothrow_move_constructible_v<U>)
            : has_value(true), value() {
            static_assert(sizeof(U) > 0, "Option<T>: T must be a complete type");

            value.construct<T>(std::move(val));
        }

        
        template<typename U = T>
            requires (std::is_reference_v<T>)
        Option(U val) noexcept : has_value(true), value() {
            static_assert(sizeof(U) > 0, "Option<T>: T must be a complete type");

            value.construct<T>(val);
        }

      
        Option(const Option& other) requires std::copy_constructible<T>
            : has_value(other.has_value), value() {
            if (has_value) {
                value.construct<T>(other.value.get<T>());
            }
        }

       
        Option(Option&& other) noexcept(std::is_nothrow_move_constructible_v<T>) requires std::move_constructible<T>
            : has_value(other.has_value),value() {
            if (has_value) {
                value.construct<T>(std::move(other.value.get<T>()));
                other.has_value = false;
          
            }
        }

      
        template<typename... Args>
            requires (!std::is_reference_v<T>)
        void emplace(Args&&... args) {
            clear();
            value.construct<T>(std::forward<Args>(args)...);
            has_value = true;
        }

        Option& operator=(const Option& other) requires std::copy_constructible<T>{
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

       
        Option& operator=(Option&& other) noexcept(std::is_nothrow_move_assignable_v<T>) {
            if (this != &other) {
                //non_null_reset
                if (has_value) {
                    value.clear<T>();
                }
                has_value = other.has_value;
                if (has_value) {
                    value.construct<T>(std::move(other.value.get<T>()));
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
        //throws if None
        T& operator()() {
            expect("Tried to acess the value of a None");
            return value.get<T>();
        }

        //throws if the Option is none
        const T& operator()() const {
            expect("Tried to acess the value of a None");
            return value.get<T>();
        }
        //throws if the Option is none
        T& unwrap() {
            expect("Called unwrap on None");
            return value.get<T>();
        }
        //throws if the Option is none
        const T& unwrap() const {
            expect("Called unwrap on None");
            return value.get<T>();
        }
        //throws if the Option is none
        T& operator*() {
            expect("Dereferencing Option with no value");
            return value.get<T>();
        }
        //throws if the Option is none
        const T& operator*() const {
            expect("Dereferencing Option with no value");
            return value.get<T>();
        }

        T unwrap_or(const T& default_val) const noexcept(std::is_nothrow_copy_constructible_v<T>) {
            return has_value ? value.get<T>() : default_val;
        }
        //throws if the Option is none
        void expect(const char* Msg = "Expected Option to have a value") const {
            if (!has_value) {
                throw std::logic_error(std::string("Option error: ") + Msg);
            }
        }

        explicit operator bool() const noexcept {
            return has_value;
        }

        bool is_some() const noexcept { return has_value; }
        bool is_none() const noexcept { return !has_value; }

        bool operator==(const Option& other) const requires std::equality_comparable<T> {
            if (has_value && other.has_value) {
                return value.get<T>() == other.value.get<T>();
            }
            return false;
        }

        bool operator!=(const Option& other) const requires std::equality_comparable<T> {
            if (has_value &&other.has_value) {
                return value.get<T>() != other.value.get<T>();
            }
            return true;
        }

        bool operator==(const T& other) const requires std::equality_comparable<T> {
            return has_value && value.get<T>() == other;
        }

        bool operator!=(const T& other) const requires std::equality_comparable<T> {
            return !(*this == other);
        }

        bool operator==(NoneType) const {
            return is_none();
        }

        bool operator!=(NoneType) const {
            return is_some();
        }
        template<typename Some_Func, typename None_Func>
        auto match(Some_Func&& some_function, None_Func&& none_function) const requires std::invocable<Some_Func,T>&&std::invocable<None_Func>
        {
            if (has_value)
            {
                return std::invoke(std::forward<Some_Func>(some_function),value.get<T>());
            }
            return std::invoke(std::forward<None_Func>(none_function));

        }
       
        template<typename Func>
        auto filter(Func pred) const -> Option<T> requires std::invoke_r<bool,Func, T> {
        if (has_value && pred(value.get<T>())) {
                return Option<T>(value.get<T>());
            }
            else {
                return stn::None;  // empty Option<U>
            }
        }
        template<typename Func, typename Default>
        auto if_or(Func&& f, Default&& default_value) const -> std::invoke_result_t<Func,T> requires std::invocable<Func, T>&&
            std::is_convertible_v<Default, std::invoke_result_t<Func, T>>{
            if (has_value) {
                return std::invoke(std::forward<Func>(f),value.get<T>());
            }
            else {
                return std::forward<Default>(default_value);  // empty Option<U>
            }
        }
        template<typename Func>
        auto map(Func&& f) const -> Option< std::invoke_result_t<Func, T>> requires std::invocable<Func,T>&& !std::is_void_v<std::invoke_result_t<Func&&, T>>{
            using U = std::invoke_result_t<Func, T>;
            if (has_value) {
                return Option<U>(std::invoke(std::forward<Func>(f), value.get<T>()));
            }
            else {
                return stn::None;  // empty Option<U>
            }
        }
      
        
    private:
        bool has_value;
        stn::erasure::erased<T> value;
    };

    template<typename T, typename... Args>
    Option<T> Construct(Args&&... args) {
        return Option<T>(T(std::forward<Args>(args)...));
    }
}
#include <format>
namespace std {

    template<typename T, typename CharT>
    struct formatter<stn::Option<T>, CharT> : formatter<T, CharT> {
        template<typename FormatContext>
        auto format(const stn::Option<T>& opt, FormatContext& ctx) requires formattable<T, CharT> {
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