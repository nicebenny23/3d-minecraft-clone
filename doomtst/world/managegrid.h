#pragma once
#include "../game/ecs/multi_query.h"
#include "grid.h"
#include "../block/air.h"
#include "../game/GameContext.h"
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
		set_block_command(Coord position, block_id blk_id) :id(blk_id), pos(position) {
		}
	};
	inline void set_block(ecs::Ecs& world, Coord pos, block_id block_id) {
		world.write_command<set_block_command>(set_block_command(pos, block_id));
	}

	struct GridManager :ecs::System {

		GridManager() {
		}
		//removes a block from the grid whilst still keeping it in the work
		void dislocate_from_grid(ecs::obj blk, block_id new_id) {
			auto& blk_comp = blk.get_component<block>();
			auto position = blk_comp.pos;
			ecs::obj& to_flip = *CtxName::ctx.Grid->getObject(position);
			to_flip = GenerateBlock(CtxName::ctx.Grid->GetChunk(position)->owner().get_component<Chunk::chunkmesh>(), new_id, position, blk_comp.mesh.direction, blk_comp.mesh.attached_direction).spawn(blk.world());
			blk.destroy();

		}



		void blockchangecoverupdate(ecs::Ecs& world, blocks::block* location) {
			world.write_command<cover_block_command>(cover_block_command(location->pos));
			for (math::Direction3d block_dir : math::Directions3d) {

				world.write_command<cover_block_command>(cover_block_command(location->pos + block_dir.coord()));
			}
		}


		//the set_block function is what should be used
		void set_block(Coord loc, block_id blockid, grid::Grid& grid, ecs::Ecs& world);




		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>().unwrap();
			for (set_block_command& cmd : world.read_commands<set_block_command>()) {
				set_block(cmd.pos, cmd.id, grid, world);
			}
			grid.updateborders();
			grid.load(world);
		}

	};
}