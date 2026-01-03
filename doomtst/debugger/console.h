// console.h
#pragma once

#include <array>
#include <string>
#include <atomic>
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

        // Log a simple message
        void Log(LogLevel level, std::string&& message);

        // Clear all entries
        void Clear();

        // Render the console UI (call each frame)
        void Render();

        // Disable copy
        Console(const Console&) = delete;
        Console& operator=(const Console&) = delete;

    private:
        Console() = default;
        ~Console() = default;

        std::array<LogEntry, MaxEntries> buffer_;
        std::atomic<int> head_{ 0 };  
        std::atomic<int> count_{ 0 };     // Number of valid entries
        ImGuiTextFilter filter_;        // For filtering displayed lines
    };

} // namespace console

// Macros for logging with file/line info and formatting
template<typename... Args>
inline void debug(Args&&... args) {
    std::stringstream ss;
    (ss << ... << std::forward<Args>(args)); // fold expression
    console::Console::Instance().Log(console::LogLevel::Info, ss.str());
}

template<typename... Args>
inline void warn(Args&&... args) {
    std::stringstream ss;
    (ss << ... << std::forward<Args>(args));
    console::Console::Instance().Log(console::LogLevel::Warning, ss.str());
}

template<typename... Args>
inline void alert(Args&&... args) {
    std::stringstream ss;
    (ss << ... << std::forward<Args>(args));
    console::Console::Instance().Log(console::LogLevel::Error, ss.str());
}
