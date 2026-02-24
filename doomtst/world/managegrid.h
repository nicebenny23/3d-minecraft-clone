#pragma once
#include "../game/ecs/multi_query.h"
#include "grid.h"
#include "../block/air.h"

#include "../util/mutex.h"
#include "../util/cached.h"
#include <cstdint>
#include <tuple>
#include <utility>
#include "../block/block.h"
#include "../game/ecs/ecs.h"
#include "../game/ecs/game_object.h"
#include "../game/ecs/query.h"
#include "../game/ecs/system.h"
#include "../math/dir.h"
#include "../util/Option.h"
#include "../util/queue.h"
#include "../math/vector3.h"
#include "Lighter.h"
#include "WorldCoverer.h"
#pragma once
namespace grid {
	struct set_block_command {
		Coord pos;
		block_id id;
		math::Direction3d attach_direction;
		set_block_command(Coord position, block_id blk_id, math::Direction3d direction) :id(blk_id), pos(position),attach_direction(direction){
		}
	};
	inline void set_block(ecs::Ecs& world, Coord pos, block_id block_id, math::Direction3d attach_direction= math::up_3d) {
		world.write_command<set_block_command>(set_block_command(pos, block_id,attach_direction));
	}
	struct GridManager :ecs::System {

		GridManager() {
		}
		//removes a block from the grid whilst still keeping it in the work
		void dislocate_from_grid(ecs::obj blk, block_id new_id,math::Direction3d attach_direction) {
			grid::Grid& grid = blk.world().get_resource<grid::Grid>();
			auto& blk_comp = blk.get_component<block>();
			auto position = blk_comp.pos;
			ecs::Constrained<block>& to_flip = grid.get_object(position).unwrap();
			to_flip = GenerateBlock{
				.id = new_id,
				.loc = position,
				.face = math::up2d,
				.direction = attach_direction,
				.mesh= grid.GetChunk(position)->owner().get_component<Chunk::chunkmesh>()
			}.spawn(blk.world());
			
			blk.destroy();
		}



		void block_change_updates(ecs::Ecs& world, Chunk::block_object& location) {
			blocks::block& blk = location.get<blocks::block>();
			world.write_command(cover_block_command(blk.pos));
			for (math::Direction3d block_dir : math::Directions3d) {

				world.write_command(cover_block_command(blk.pos + block_dir.coord()));			}
		}

		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>();
			for (set_block_command& cmd : world.read_commands<set_block_command>()) {

				stn::Option<Chunk::block_object&> mabye_location = grid.get_object(cmd.pos);
				if (mabye_location) {
					Chunk::block_object& location = mabye_location.unwrap();
					grid.GetChunk(cmd.pos)->modified = true;
					size_t old_light = location.get<block>().light_passing_through;
					world.write_command(partial_darken_command{ .location = cmd.pos, .old_light = old_light});
					dislocate_from_grid(location.object(), cmd.id, cmd.attach_direction);
					location = grid.get_object(cmd.pos).unwrap();
					block_change_updates(world, location);
				}
			}
			grid.updateborders();
			grid.load(world);
		}

	};
}