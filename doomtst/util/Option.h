#pragma once
#include <utility>  // for std::move#include <string>   // for std::string in exception message
namespace Opt {

    struct NoneType {
        struct NullTag {};
        constexpr explicit NoneType(NullTag) {};
    };

    inline constexpr NoneType None{NoneType::NullTag{}};


    template<typename T>
    struct Option {
    private:
        bool has_value;
     
        T value;

    public:
        // Default constructor: no value
        Option() noexcept : has_value(false) {}
        Option(NoneType) noexcept : has_value(false) {}
        // Construct with const lvalue reference
        Option(const T& val) : has_value(true), value(val) {}

        // Construct with rvalue reference (move)
        Option(T&& val) noexcept(std::is_nothrow_move_constructible_v<T>)
            : has_value(true), value(std::move(val)) {}

        // Copy constructor
        Option(const Option& other)
            : has_value(other.has_value)
        {
            if (has_value) {
                value = other.value;
            }
        }

        // Move constructor
        Option(Option&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
            : has_value(other.has_value)
        {
            if (has_value) {
                value = std::move(other.value);
                other.clear();
            }
        }

        template<typename... Args>
        void emplace(Args&&... args) {
            clear(); 
            value = T(std::forward<Args>(args)...);
            has_value = true;
        }

        // Copy assignment operator
        Option& operator=(const Option& other) {
            if (this != &other) {
                has_value = other.has_value;
                if (has_value) {
                    value = other.value;
                }
            }
            return *this;
        }

        // Move assignment operator
        Option& operator=(Option&& other) noexcept(std::is_nothrow_move_assignable_v<T>) {
            if (this != &other) {
                has_value = other.has_value;
                if (has_value) {
                    value = std::move(other.value);
                    other.clear();
                }
            }
            return *this;
        }
        Option& operator=(NoneType){
            clear();
            return *this;
        }
        void clear() {
            has_value = false;
        }


        template<typename Func>
        auto and_then(Func f) const -> Option<decltype(f(std::declval<T>()))> {
            using U = decltype(f(std::declval<T>()));
            if (has_value) {
                return Option<U>(f(value));
            }
            else {
                return Option<U>(None);  // empty Option<U>
            }
        }
        template<typename Func>
        auto filter(Func pred) const -> Option<T> {
       
            if (has_value&&filter(value)) {
                return Option<T>(value);
            }
            else {
                return Option<T>(None);  // empty Option<U>
            }
        }
      template<typename Func>
        auto fold_or(Func f, decltype(f(std::declval<T>())) def) const -> decltype(f(std::declval<T>())) {
            using U = decltype(f(std::declval<T>()));
            if (has_value) {
                return (f(value));
            }
            else {
                return def;  // empty Option<U>
            }
        }
        // Get value (throws if None)
        T& unwrap() {
            expect("Called unwrap on None");
            return value;
        }

        const T& unwrap() const {
            expect("Called unwrap on None");
            return value;
        }

        T& operator*() {
            expect("Dereferencing Option with no value");
            return value;
        }

        const T& operator*() const {
            expect("Dereferencing Option with no value");
            return value;
        }

      

     
        // Return value or default if None
        T unwrap_or(const T& default_val) const noexcept(std::is_nothrow_copy_constructible_v<T>) {
            return has_value ? value : default_val;
        }

        // Throw with custom message if no value
        
      inline void expect(const char* Msg= "Expected Option to have a value") const {
            if (!has_value) {
                throw std::logic_error(std::string("Option error: ") + Msg);
            }
        }


        // Check if Option has a value
        explicit operator bool() const noexcept {
            return has_value;
        }

        bool is_some() const noexcept { return has_value; }
        bool is_none() const noexcept { return !has_value; }


        bool operator==(const Option& other) const {
            if (has_value != other.has_value) return false;
            return !has_value || value == other.value;
        }

        bool operator!=(const Option& other) const {
            if (has_value != other.has_value) return true;
            return has_value && value != other.value;
        }
        
        bool operator==(const T& other) const {
            return has_value && value == other;
        }

        bool operator!=(const T& other) const {
            return !(*this == other);
        }
        bool operator==(NoneType) const {
            return is_none();
        }
        bool operator!=(NoneType) const {
            return is_some();
        }
    };


    template<typename T, typename... Args>
    Option<T> Construct(Args&&... args) {
        return Option<T>(T(std::forward<Args>(args)...));
    }
}