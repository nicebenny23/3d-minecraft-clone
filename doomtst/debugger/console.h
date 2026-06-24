// console.h
#pragma once
#include <array>
#include <string>
#include <atomic>
#include "../renderer/Window.h"
#include <format>
#include "../imgui/imgui.h"
#include <sstream>
//i dont get imgui so this is not mine
namespace console {

    // Log severity levels
    enum class LogLevel { Info, Warning, Error };

    // Single log entry
    struct LogEntry {
        LogLevel level;
        std::string text;
    };

    class Console {
    public:
        static constexpr int MaxEntries = 1024;

        static Console& Instance();

        void log(LogLevel level, std::string&& message);

        void clear();

        void render();

        // Disable copy
        Console(const Console&) = delete;
        Console& operator=(const Console&) = delete;

    private:
        Console() = default;
        ~Console() = default;
        std::array<LogEntry, MaxEntries> buffer_;
        std::atomic<int> head_{ 0 };  
        std::atomic<int> count_{ 0 };    
        ImGuiTextFilter filter_;
		uint32_t input_id = 0;
    };

} 

template<typename... Args>
inline void debug(Args&&... args) {
    std::stringstream ss;
    (ss << ... << std::forward<Args>(args)); 
    console::Console::Instance().log(console::LogLevel::Info, ss.str());
}
template<typename... Args>
inline void debug_fmt(const std::format_string<Args...>& fmt, Args&&... args) {
	console::Console::Instance().log(console::LogLevel::Info,std::format(fmt, std::forward<Args>(args)...));
}
template<typename... Args>
inline void warn(Args&&... args) {
    std::stringstream ss;
    (ss << ... << std::forward<Args>(args));
    console::Console::Instance().log(console::LogLevel::Warning, ss.str());
}

template<typename... Args>
inline void alert(Args&&... args) {
    std::stringstream ss;
    (ss << ... << std::forward<Args>(args));
    console::Console::Instance().log(console::LogLevel::Error, ss.str());
}
