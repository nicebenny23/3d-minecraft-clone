#include "../game/GameContext.h"
#include "console.h"
#include "../player/player.h"
#include <format>
#include "../player/cameracomp.h"
namespace console {

    Console& Console::Instance() {
        static Console instance;
        return instance;
    }

    void Console::Log(LogLevel level, std::string&& message) {
        int idx = head_.fetch_add(1, std::memory_order_relaxed) % MaxEntries;
        buffer_[idx] = { level, std::move(message) };
        int c = count_.load(std::memory_order_relaxed);
        if (c < MaxEntries) {
            count_.store(c + 1, std::memory_order_relaxed);
        }
    }

    void Console::Clear() {
        head_.store(0, std::memory_order_relaxed);
        count_.store(0, std::memory_order_relaxed);
    }

    void Console::Render() {
        filter_.Draw("Filter");

        ImGui::BeginChild("ConsoleOutput", ImVec2(0, 200), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        float scrollY = ImGui::GetScrollY();
        float scrollMax = ImGui::GetScrollMaxY();

        int c = count_.load(std::memory_order_relaxed);
        int h = head_.load(std::memory_order_relaxed);
        for (int i = 0; i < c; ++i) {
            int idx = (h - c + i + MaxEntries) % MaxEntries;
            const auto& entry = buffer_[idx];
            const char* text = entry.text.c_str();
            if (!filter_.PassFilter(text))
                continue;

            ImVec4 color;
            switch (entry.level) {
            case LogLevel::Info:    color = ImVec4(1, 1, 1, 1); break;
            case LogLevel::Warning: color = ImVec4(1, 1, 0, 1); break;
            case LogLevel::Error:   color = ImVec4(1, 0, 0, 1); break;
            }
            ImGui::TextColored(color, "%s", text);
        }

        if (scrollY >= scrollMax - 1.0f)
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::BeginChild("ConsoleControls", ImVec2(0, 50), true);
        
        if (ImGui::Button("Clear")) {
            Clear();
        }
        ImGui::EndChild();
        ImGui::BeginChild("Debug Info"); 
        ImGui::Text("Debug");
        ImGui::Text(std::format("Fps {:.3f}", CtxName::ctx.Time->smooth_fps).c_str());
        ImGui::Text(std::format("position: {}", player::goblin.getcomponent<gameobject::transform_comp>().transform.position).c_str());

        ImGui::Text(std::format("yaw:{:.3f},pitch:{:.3f}", player::goblin.getcomponent<CameraComp>().CamTransform.yaw, player::goblin.getcomponent<CameraComp>().CamTransform.pitch).c_str());
        ImGui::EndChild();
    }

} // namespace console
