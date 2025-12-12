#include "../game/GameContext.h"
#include "console.h"
#include "../player/player.h"
#include <format>
#include "../player/cameracomp.h"
#include "../player/playermodification.h"
#include "../player/playerplace.h"
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
        if (ImGui::IsWindowCollapsed()) {
            ImGui::Text("Window is collapsed! Click arrow to expand.");
        }
       
        if (ImGui::BeginTabBar("DebugTabs")) {
            if (ImGui::BeginTabItem("Console")) {

                ImGui::BeginChild("Debug Info", ImVec2(0, 150), true);
                ImGui::Text("Debug:");
                ImGui::Text(std::format("Fps: {:.3f}", CtxName::ctx.Time->smooth_fps).c_str());

                Vec3 pos = player::goblin.get_component<ecs::transform_comp>().transform.position;
                ImGui::Text(std::format("position: {}", pos).c_str());

                ImGui::Text(std::format("yaw:{:.3f},pitch:{:.3f}", player::goblin.get_component<CameraComp>().CamTransform.yaw, player::goblin.get_component<CameraComp>().CamTransform.pitch).c_str());
                ImGui::Text(std::format("Chunk: {}", CtxName::ctx.Grid->chunkfromblockpos(Coord(pos))).c_str());
                std::string text_for_look = player::goblin.get_component<playerplace>().Hit
                    .filter([&](const voxtra::RayWorldHit& blk) {return blk.ecs().exists() && blk.ecs().has_component<block>(); })
                    .map([&](const voxtra::RayWorldHit& blk) {return std::format("looking at {}", blk.ecs().get_component<block>().pos); })
                    .unwrap_or("not looking at a block");
                
                ImGui::Text(text_for_look.c_str());
                ImGui::EndChild();

                ImGui::EndTabItem();  // <-- Add this!
            }

            if (ImGui::BeginTabItem("Search")) {
                filter_.Draw("Filter");
                ImGui::SameLine();
                if (ImGui::RadioButton("Clear", true)) {
                    Clear();
                }

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

                ImGui::EndTabItem();  // <-- Add this!
            }

            ImGui::EndTabBar();
        }
    }

} // namespace console
