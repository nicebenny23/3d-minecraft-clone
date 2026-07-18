#pragma once

#include "block_registry.h"
#include "../items/loottable.h"
namespace items {
	
	struct DirtItem :item_type{

		std::string name() const {
			return "dirt";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits::block_item(renderer::TexturePath("images\\dirt.png"),
				world.get_resource<blocks::BlockRegistry>().get_id("soil"));
		}

	};
	struct MulchItem :item_type {

		std::string name() const {
			return "mulch";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits{.image_path=renderer::TexturePath("images\\mulch.png"),.fertilizer=1};
		}

	};
	struct soil_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types,ecs::obj dropping) const {
			return items::LootDrops({ items::loot_element(types.insert<DirtItem>(),1,types) });
		}
	};

}
namespace blocks {

	struct Seedability :ecs::component {
		Seedability(size_t amt) :seedable(amt) {

		}
		size_t seedable;
		bool need_reset= true;
		void set_sd(size_t amt) {
			seedable = amt;
			need_reset = true;
		}
	};
	struct FertileTexturer:ecs::System {
		void run(ecs::Ecs& world) {
			BlockTextureRegistry& textures = world.get_resource<BlockRegistry>().textures;
			for (auto[fertile,block]:ecs::View<Seedability,block>(world)) {
				block_texture texture;
				if (fertile.need_reset) {
					if (fertile.seedable == 0) {
						texture = textures.get_texture("images\\silt.png");
					}
					else {
						texture = textures.get_texture("images\\dirt.png");
					}
					for (math::Direction3d dir : math::Directions3d) {
						block.mesh.set_face_texture(dir, texture);
					}
					fertile.need_reset = false;
				}
				
			}
		}
	};
	struct SoilBlock :BlockType {
		void apply(ecs::obj& block) const {

			block.apply_recipe(items::loot_table_recipe<items::soil_loot_table>);
			block.add_component<Seedability>(0);
		}
		std::string name() const override {
			return std::string("soil");
		}
		stn::Option<SolidBlockTraits> solid_traits_for() const override {
			return SolidBlockTraits(1,0,false);
		}
		BlockMeshTraits traits(BlockTextureRegistry& textures)const {
			return  BlockMeshTraits(v3::unit_scale, false, textures.get_texture("images\\dirt.png"));
		}
		void read_from_bytes(ecs::obj block, stn::file_handle& handle)const  override {
			block.apply_recipe(items::loot_table_recipe<items::soil_loot_table>);
			block.add_component<Seedability>(stn::file_serializer<size_t>().read(handle));
		}
		void write_to_bytes(ecs::obj block, stn::file_handle& handle)const  override {

			size_t seedable=block.get_component<Seedability>().seedable;
			stn::file_serializer<size_t>().write(seedable, handle);
		}
	};
	inline void soil_plugin(core::App& app) {
		app.emplace_system<FertileTexturer>();
	}
	
}
