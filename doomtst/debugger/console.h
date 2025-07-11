// console.h
#pragma once

#include <array>
#include <string>
#include <atomic>
#include <format>
#include "../imgui/imgui.h"
#include <sstream>
namespace console {

    // Log severity levels
    enum class LogLevel { Info, Warning, Error };

    // Single log entry
    struct LogEntry {
        LogLevel level;
        std::string text;
    };

    // Console singleton: fixed-size ring buffer
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
#define debug(...) do { std::stringstream ss{}; ss << __VA_ARGS__; \
    console::Console::Instance().Log(console::LogLevel::Info, ss.str()); } while (0)
#define warn(...) do { std::stringstream ss{}; ss << __VA_ARGS__; \
    console::Console::Instance().Log(console::LogLevel::Warning, ss.str()); } while (0)
#define alert(...) do { std::stringstream ss{}; ss << __VA_ARGS__; \
    console::Console::Instance().Log(console::LogLevel::Error, ss.str()); } while (0)

