#include "block.h"
#include "block_registry.h"
#include "../items/loottable.h"
#include "plank.h"
#include "../world/managegrid.h"
#include "soil.h"
#pragma once 
namespace items {

	struct SeedItem : item_type {
		std::string name() const {
			return "seed";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits{ .image_path =renderer::TexturePath("images\\seed.png"),.blk_id = world.get_resource<BlockRegistry>().get_id("seed")};
		}
	};

}
namespace blocks {
	constexpr double moss_lifetime = 200;
	struct SeedLifetime :ecs::component {
		timing::Duration clock;
		void prime() {
			clock.set(moss_lifetime);
		}
		SeedLifetime(timing::WorldClock& world_clock, double lifetime) :clock(world_clock) {
			clock.set(lifetime);
		}
	};
	struct SeedGrower :ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View<SeedLifetime, block> moss_clock(world);
			grid::Grid& grid = world.get_resource<grid::Grid>();
			for (auto&& [moss, mblock] : moss_clock) {
				if (!grid::lights_loaded_at(grid, mblock.pos)) {
					continue;
				}

				bool stop = true;
				for (math::Direction3d dir : math::Directions3d) {
					stn::Option<blocks::block&> blk = grid.get_block(dir.coord() + mblock.pos);
					if (blk && blk.unwrap().light_passing_through != 0) {
						stop = false;
					}
				}
				stn::Option<ecs::Constrained<block>&> blk_below = grid.get_object(mblock.pos - v3::Coord(0, 1, 0));
				if (blk_below) {
					int seed = blk_below.unwrap().get_component_opt<Seedability>().member(&Seedability::seedable).unwrap_or(0);
					if (seed<1) {

						stop = true;
					}
				}
				else {
					stn::throw_logic_error("for lights to be loaded blk_below must be loaded thus it should be loaded");
				}
				BlockTextureRegistry& textures = world.get_resource<BlockRegistry>().textures;
				block_texture general_tex = textures.get_texture("images\\moss.png");
				if (stop) {
					general_tex = textures.get_texture("images\\dead_moss.png");
					moss.prime();
				}

				stn::List<block_texture, 4> tex_array{ general_tex,textures.get_texture("images\\moss_one.png"),textures.get_texture("images\\moss_two.png"),textures.get_texture("images\\moss_three.png") };
				if (!stop) {
					size_t tex_index = std::floor(math::bounds(0, moss_lifetime).unlerp_clamped(moss_lifetime - moss.clock.remaining_or_default()) * 3.999f);
					general_tex = tex_array[tex_index];
				}
				for (math::Direction3d dir : math::Directions3d) {
						mblock.mesh.set_face_texture(dir, general_tex);
				}


				if (moss.clock.is_inactive()) {
					grid.get_object(mblock.pos - v3::Coord(0, 1, 0)).unwrap().get_component<Seedability>().seedable-=1;
					grid::set_block(world, mblock.pos, world.get_resource<BlockRegistry>().get_id<LogBlock>());
				}
			}
		}
	};

	struct seed_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types,ecs::obj dropping) const {
			return items::LootDrops({ items::loot_element(types.insert<items::SeedItem>(),1,types) });
		}
	};
	struct SeedBlock :BlockType {
		void apply(ecs::obj& block) const override {
			block.add_component<SeedLifetime>(block.world().get_resource<timing::WorldClock>(), moss_lifetime);
			block.apply_recipe(items::loot_table_recipe< seed_loot_table>);
		}
		std::string name() const {
			return std::string("seed");
		}
		void read_from_bytes(ecs::obj block, stn::file_handle& handle)const  override {
			block.apply_recipe(items::loot_table_recipe< seed_loot_table>);
			timing::WorldClock& clock = block.world().get_resource<timing::WorldClock>();
			block.add_component<SeedLifetime>(clock, stn::file_serializer<double>().read(handle) - clock.elapsed_time);
		}
		void write_to_bytes(ecs::obj block, stn::file_handle& handle)const  override {
			SeedLifetime& lifetime = block.get_component<SeedLifetime>();
			stn::file_serializer<double>().write(lifetime.clock.end_time().unwrap_or(0), handle);
		}
		BlockMeshTraits traits(BlockTextureRegistry& textures) const {
			return BlockMeshTraits(v3::unit_scale, false, textures.get_texture("images\\moss.png"));
		}
		blocks::SolidBlockTraits mining_traits() const {
			return blocks::SolidBlockTraits(2, 0, false);
		}
	};
}