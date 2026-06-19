#include "block.h"
#include "block_registry.h"
#include "../items/loottable.h"
#include "plank.h"
#include "../world/managegrid.h"
#pragma once 
namespace items {

	struct seed_item : item_type {
		std::string name() const {
			return "seed";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				renderer::TexturePath("images\\seed.png"),
				world.get_resource<BlockRegistry>().get_id("seed"));
		}
	};

}
namespace blocks {

	struct SeedLifetime :ecs::component {
		timing::Duration clock;
		void prime() {
			clock.set(300);
		}
		SeedLifetime(timing::WorldClock& world_clock,double lifetime) :clock(world_clock) {
			clock.set(lifetime);
		}
	};
	struct SeedGrower:ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View<SeedLifetime,block> moss_clock(world);
			grid::Grid& grid=world.get_resource<grid::Grid>();
			for (auto&& [moss,block]: moss_clock) {
				if (!grid::lights_loaded_at(grid, block.pos)) {
					continue;
				}
				bool stop = true;
				for (math::Direction3d dir : math::Directions3d) {
					stn::Option<blocks::block&> blk = world.get_resource<grid::Grid>().get_block(dir.coord() + block.pos);
					if (blk && blk.unwrap().light_passing_through != 0) {
						stop = false;
					}
				}
				block_texture general_tex = mosstex;
				if (stop) {
					general_tex = moss_inactive_tex;
						moss.prime();
				}
				
				block_texture side_tex = general_tex;
				stn::List<block_texture, 4> tex_array{ mosstex,moss_one,moss_two,moss_three };
				if (!stop) {
					size_t tex_index =std::floor(math::bounds(0,300).unlerp_clamped(300-moss.clock.remaining().unwrap())*3.999f);
					side_tex = tex_array[tex_index];
				}
				for (Direction3d dir : math::Directions3d) {
					if (dir.axis()==math::AxisIndex3d::Up) {

						block.mesh.set_face_texture(dir, general_tex);
					}
					else {
						block.mesh.set_face_texture(dir,side_tex);
					}
					}
			

				if (moss.clock.is_inactive()) {
					grid::set_block(world,block.pos,world.get_resource<BlockRegistry>().get_id<LogBlock>());
				}
			}
		}
	};

	struct seed_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types) {
			return items::LootDrops({ items::loot_element(types.insert<items::seed_item>(),1,types) });
		}
	};
	struct SeedBlock :BlockType  {
		void apply(ecs::obj& block) const override {
			block.add_component<SeedLifetime>(block.world().get_resource<timing::WorldClock>(),300);
			block.apply_recipe(items::loot_table_recipe< seed_loot_table>);
		}
		std::string name() const {
			return std::string("seed");
		}
		void read_from_bytes(ecs::obj block, stn::file_handle& handle)const  override {
			block.apply_recipe(items::loot_table_recipe< seed_loot_table>);
			timing::WorldClock& clock= block.world().get_resource<timing::WorldClock>();
			block.add_component<SeedLifetime>(clock, stn::file_serializer<double>().read(handle)- clock.elapsed_time);
		}
		void write_to_bytes(ecs::obj block, stn::file_handle& handle)const  override {
			SeedLifetime& lifetime=block.get_component<SeedLifetime>();
			stn::file_serializer<double>().write(lifetime.clock.end_time().unwrap_or(0), handle);
		}
		BlockTraits traits() const {
			return BlockTraits(
				BlockMeshTraits(v3::unit_scale, false, mosstex)
			);

		}
		blocks::SolidBlockTraits mining_traits() const {
			return blocks::SolidBlockTraits(2,0,false);
		}
	};
}