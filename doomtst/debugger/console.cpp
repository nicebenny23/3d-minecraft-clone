
#include "console.h"
#include "../player/player.h"
#include <format>
#include "../player/cameracomp.h"
#include "../player/playermodification.h"
#include "../game/Settings.h"
#include "../player/playerplace.h"
#include "../game/time.h"

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

	void Console::clear() {
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
				
				ImGui::Text(std::format("Fps: {:.3f}", player::goblin.world().ensure_resource<timing::WorldClock>().fps()).c_str());

				Point3 pos = player::goblin.get_component<core::LocalTransform>().transform.position;
				ImGui::Text(std::format("position: {}", pos).c_str());
				grid::Grid& grid = player::goblin.world().get_resource<grid::Grid>();
				ImGui::Text(std::format("look: {}", player::goblin.get_component<core::LocalTransform>().transform.look).c_str());
				ImGui::Text(std::format("Chunk: {}", grid.chunk_from_block_pos(Coord(pos)).position).c_str());
				std::string text_for_look = player::goblin.get_component<player::PlayerCursor>().hit
					.filter([&](const voxtra::RayWorldHit& blk) {return blk.owner().exists(); })
					.filter([](const voxtra::RayWorldHit& blk) {return blk.owner().has_component<block>(); })
					.map([&](const voxtra::RayWorldHit& blk) {
					block& blck = blk.owner().get_component<block>();

					return std::format("looking at block with center {},name {}", blck.center(), blck.type()->name()); })
					.unwrap_or("not looking at a block");

				ImGui::Text(text_for_look.c_str());
				ImGui::Text(std::format("loaded chunks{}", grid.chunks_loaded()).c_str());
				ImGui::EndChild();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Entity Viewer")) {

				ImGui::InputScalar("Entity ID", ImGuiDataType_U32, &input_id);

				ecs::entity_id entity(input_id);
				/*if (CtxName::ctx.Ecs) {
					auto types = CtxName::ctx.Ecs->component_types_for(entity);

					for (const auto& type : types) {
						ImGui::Text("%s", type->component_type_id().name().data());
					}
				}
				*/
				ImGui::EndTabItem();
			}


			if (ImGui::BeginTabItem("Search")) {
				filter_.Draw("Filter");
				ImGui::SameLine();
				if (ImGui::RadioButton("Clear", true)) {
					clear();
				}
				if (ImGui::RadioButton("set_view", false)) {
					player::goblin.world().ensure_resource<settings::GlobalSettings>().viewmode ^= true;
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

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}

} // namespace console
