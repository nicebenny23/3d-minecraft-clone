#include <format>
#include <exception>
#include <source_location>
#pragma once
namespace stn {

    template<typename... Args>
    [[noreturn]] void throw_range_exception(const std::format_string<Args...>& fmt, Args&&... args) {
        throw std::out_of_range(std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    [[noreturn]] void throw_logic_error(const std::format_string<Args...>& fmt, Args&&... args) {
        throw std::logic_error(std::format(fmt, std::forward<Args>(args)...));
    }

    // Other standard exception types
    template<typename... Args>
    [[noreturn]] void throw_invalid_argument(const std::format_string<Args...>& fmt, Args&&... args) {
        throw std::invalid_argument(std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    [[noreturn]] void throw_length_error(const std::format_string<Args...>& fmt, Args&&... args) {
        throw std::length_error(std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    [[noreturn]] void throw_out_of_range(const std::format_string<Args...>& fmt, Args&&... args) {
        throw std::out_of_range(std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    [[noreturn]] void throw_domain_error(const std::format_string<Args...>& fmt, Args&&... args) {
        throw std::domain_error(std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    [[noreturn]] void throw_runtime_error(const std::format_string<Args...>& fmt, Args&&... args) {
        throw std::runtime_error(std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    [[noreturn]] void throw_system_error(int code, const std::format_string<Args...>& fmt, Args&&... args) {
        throw std::system_error(code, std::generic_category(), std::format(fmt, std::forward<Args>(args)...));
    }

    //exception for assertion_debugging
    class assert_error : public std::exception {
    public:        
        const char* what() const noexcept override {
            return msg.c_str();
        } 
    private:
        explicit assert_error(std::string message)
            : msg(std::move(message)) {
        }
        template<typename... Args>
        friend void assert_exception(bool condition,const std::format_string<Args...>& fmt,Args&&... args);

        std::string msg;
    };
    

    template<typename... Args>  
    inline void assert_exception(bool condition,const std::format_string<Args...>& fmt,Args&&... args)
    {
        if (!condition) {     
            throw assert_error(std::format(fmt, std::forward<Args>(args)...));
        }
    }
}