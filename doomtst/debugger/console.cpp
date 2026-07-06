
#include "console.h"
#include "../player/player.h"
#include <format>
#include "../player/cameracomp.h"
#include "../player/playermodification.h"
#include "../game/Settings.h"
#include "../player/playerplace.h"
#include "../game/time.h"
#include "../world/grid.h"
#include "../game/ecs/game_object.h"

namespace console {
	void Console::render() {
		if (ImGui::IsWindowCollapsed()) {
			ImGui::Text("Window is collapsed! Click arrow to expand.");
		}

		if (ImGui::BeginTabBar("DebugTabs")) {
			if (ImGui::BeginTabItem("Console")) {

				ImGui::BeginChild("Debug Info", ImVec2(0, 150), true);
				ImGui::Text("Debug:");
				ecs::obj goblin = player::player_for(world);
				ImGui::Text(std::format("Fps: {:.3f}", world.ensure_resource<timing::WorldClock>().fps()).c_str());
				ImGui::Text(std::format("Elapsed Time: {:.0f}", world.ensure_resource<timing::WorldClock>().elapsed_time).c_str());
				Point3 pos = player::player_for(world).get_component<core::LocalTransform>().transform.position;
				ImGui::Text(std::format("position: {}", pos).c_str());
				grid::Grid& grid = world.get_resource<grid::Grid>();
				ImGui::Text(std::format("look: {}", goblin.get_component<core::LocalTransform>().transform.look).c_str());
				ImGui::Text(std::format("Chunk: {}", grid.chunk_from_block_pos(Coord::from_vec3(pos)).position).c_str());
				std::string text_for_look = goblin.get_component<player::PlayerCursor>().hit
					.filter([&](const voxtra::RayWorldHit& blk) {return blk.owner().exists(); })
					.filter([](const voxtra::RayWorldHit& blk) {return blk.owner().has_component<block>(); })
					.map([&](const voxtra::RayWorldHit& blk) {
					block& blck = blk.owner().get_component<block>();

					return std::format("looking at block with center {},name {}", blck.center(), blck.type().name()); })
					.unwrap_or("not looking at a block");

				ImGui::Text(text_for_look.c_str());
				ImGui::Text(std::format("loaded chunks{}", grid.chunks_loaded()).c_str());
				ImGui::EndChild();

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}

} // namespace console
