#pragma once
#include <utility>
#include <string>
#include <type_traits>
#include <stdexcept>

namespace Opt {

    struct NoneType {
        struct NullTag {};
        constexpr explicit NoneType(NullTag) {};
    };
    inline constexpr NoneType None{ NoneType::NullTag{} };

    template<typename T>
    struct Opt_Storage {
        Opt_Storage() noexcept = default;
        ~Opt_Storage() = default;

        alignas(T) unsigned char storage[sizeof(T)];

        T& get() {
            return *get_ptr();
        }
        const T& get() const {
            return *get_ptr();
        }
        void set(const T& val) {
            get() = val;
        }
        void set(T&& val) {
            get() = std::move(val);
        }

        template<typename... Args>
        void construct(Args&&... args) {
            static_assert(std::is_constructible_v<T, Args...>);
            new (&storage) T(std::forward<Args>(args)...);
        }
        void construct(const T& val) {
            new (&storage) T(val);
        }
        void reset() {
            get_ptr()->~T();
        }

    private:
        T* get_ptr() {
            return reinterpret_cast<T*>(storage);
        }
        const T* get_ptr() const {
            return reinterpret_cast<const T*>(storage);
        }
    };

    template<typename T>
    struct Opt_Storage<T&> {
        Opt_Storage() noexcept = default;
        ~Opt_Storage() = default;

        T* value = nullptr;

        T& get() {
            return *value;
        }
        const T& get() const {
            return *value;
        }
        void set(const T& oth) {
            value = const_cast<T*>(&oth);
        }
        void construct(const T& val) {
            value = const_cast<T*>(&val);
        }
        
        void reset() {
            value = nullptr;
        }
    };

    template<typename T>
    struct Option {
    public:
    
        Option() noexcept : has_value(false),value() {}
        Option(NoneType) noexcept : has_value(false), value() {}

        
        template<typename U = T>
            requires (!std::is_reference_v<U>)
        Option(const U& val) : has_value(true), value() {
            value.construct(val);
        }

       
        template<typename U = T>
            requires (!std::is_reference_v<U>)
        Option(U&& val) noexcept(std::is_nothrow_move_constructible_v<U>)
            : has_value(true), value() {
            value.construct(std::move(val));
        }

        
        template<typename U = T>
            requires (std::is_reference_v<U>)
        Option(U val) noexcept : has_value(true), value() {
            value.construct(val);
        }

      
        Option(const Option& other)
            : has_value(other.has_value), value() {
            if (has_value) {
                value.construct(other.value.get());
            }
        }

       
        Option(Option&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
            : has_value(other.has_value) {
            if (has_value) {
                value.construct(std::move(other.value.get()));
                other.has_value = false;
            }
        }

      
        template<typename... Args>
            requires (!std::is_reference_v<T>)
        void emplace(Args&&... args) {
            clear();
            value.construct(std::forward<Args>(args)...);
            has_value = true;
        }

        Option& operator=(const Option& other) {
            if (this != &other) {
                if (has_value) {
                    value.reset();
                }
                has_value = other.has_value;
                if (has_value) {
                    value.construct(other.value.get());
                }
            }
            return *this;
        }

       
        Option& operator=(Option&& other) noexcept(std::is_nothrow_move_assignable_v<T>) {
            if (this != &other) {
                if (has_value) {
                    value.reset();
                }
                has_value = other.has_value;
                if (has_value) {
                    value.construct(std::move(other.value.get()));
                    other.has_value = false;
                }
            }
            return *this;
        }

        Option& operator=(NoneType) {
            clear();
            return *this;
        }

        void clear() {
            if (has_value) {
                value.reset();
                has_value = false;
            }
        }

       

        T& unwrap() {
            expect("Called unwrap on None");
            return value.get();
        }

        const T& unwrap() const {
            expect("Called unwrap on None");
            return value.get();
        }

        T& operator*() {
            expect("Dereferencing Option with no value");
            return value.get();
        }

        const T& operator*() const {
            expect("Dereferencing Option with no value");
            return value.get();
        }

        T unwrap_or(const T& default_val) const noexcept(std::is_nothrow_copy_constructible_v<T>) {
            return has_value ? value.get() : default_val;
        }

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
            if (has_value != other.has_value) return false;
            return !has_value || value.get() == other.value.get();
        }

        bool operator!=(const Option& other) const requires std::equality_comparable<T> {
            return !(*this==other);
        }

        bool operator==(const T& other) const requires std::equality_comparable<T> {
            return has_value && value.get() == other;
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

        T& operator()() {
            return unwrap();
        }

        const T& operator()() const {
            return unwrap();
        }
        template<typename Func>
        auto filter(Func pred) const -> Option<T> {

            if (has_value && pred(value.get())) {
                return Option<T>(value.get());
            }
            else {
                return Opt::None;  // empty Option<U>
            }
        }

        template<typename Func>
        auto map(Func f) const -> Option<decltype(f(std::declval<T>()))> {
            using U = decltype(f(std::declval<T>()));
            if (has_value) {
                return Option<U>(f(value.get()));
            }
            else {
                return Opt::None;  // empty Option<U>
            }
        }
    private:
        bool has_value;
        Opt_Storage<T> value;
    };

    template<typename T, typename... Args>
    Option<T> Construct(Args&&... args) {
        return Option<T>(T(std::forward<Args>(args)...));
    }
}
