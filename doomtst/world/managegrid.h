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
				.block_face = math::up2d,
				.direction = attach_direction,
				.mesh= grid.get_chunk(position).unwrap().owner().get_component<chunks::ChunkMesh>(),
				.registry = blk.world().get_resource<blocks::BlockRegistry>()
			}.spawn(blk.world());
			
			blk.destroy();
		}




		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>();
			for (set_block_command& cmd : world.read_commands<set_block_command>()) {

				stn::Option<chunks::block_object&> mabye_location = grid.get_object(cmd.pos);
				if (mabye_location) {
					chunks::block_object& location = mabye_location.unwrap();
					grid.get_chunk(cmd.pos).unwrap().modified = true;
					size_t old_light = location.get<block>().light_passing_through;
					world.write_command(grid::lighten_block_command(cmd.pos));
					world.write_command(partial_darken_command{ .location = cmd.pos, .old_light = old_light});
					dislocate_from_grid(location.object(), cmd.id, cmd.attach_direction);
					location = grid.get_object(cmd.pos).unwrap();
					blocks::block& blk = location.get<blocks::block>();
					for (math::Direction3d block_dir : math::Directions3d) {
						blk.mesh.mark_dirty(block_dir);

						grid.get_block(cmd.pos+ block_dir.coord()).then([&](block& seen_block) {
							seen_block.mesh.mark_dirty(-block_dir);
							world.write_command(grid::lighten_block_command(seen_block.pos));
						});
					}
				}
			}

			grid.updateborders();
			grid.load(world);
		}

	};
}