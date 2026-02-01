#include "ItemSlot.h"
#pragma once
namespace items {
	inline constexpr v2::Coord2 grid_size = v2::Coord2(30, 30);
	inline Box2d inventory_transform_floating(v2::Vec2 center) {
		v2::Vec2 scale = v2::Vec2(1.0f / grid_size.x, 1.0f / grid_size.y);
		return Box2d(center, scale);
	}
	inline v2::Vec2 inventory_center(v2::Coord2 location) {
		float xval = (2 * location.x+1.0f) / float(grid_size.x);
		float yval = (2 * location.y+1.0f) / float(grid_size.y);
		return v2::Vec2(xval, yval);
	}
	
	struct ItemSlotDecal :ecs::component {
		ItemSlotDecal(ecs::obj decal) :item_decal_object(decal) {

		}
		void set_decal(renderer::TexturePath Path) {
			item_decal_object.get_component<ui::ui_image_component>().set_image(Path);
		}
		void reset_decal() {
			set_decal(renderer::TexturePath("images\\blockholder.png", "item_decal_original"));
		}
		ecs::obj item_decal_object;
	};


	struct FloatingItemSlotUiSpawner:ecs::Recipe {
		FloatingItemSlotUiSpawner(v2::Vec2 inv_loc):inventory_location(inv_loc) {

		}
		v2::Vec2 inventory_location;
		void apply(ecs::obj& entity) {

			ui::ui_spawner(inventory_transform_floating(inventory_location), 0).apply(entity);
		}
	};

	struct FloatingItemSlotSpawner :ecs::Recipe {
		FloatingItemSlotSpawner(v2::Vec2 inv_loc) : inventory_location(inv_loc) {

		}
		v2::Vec2 inventory_location;
		void apply(ecs::obj& entity) {
			FloatingItemSlotUiSpawner(inventory_location).apply(entity);
			ItemSlotSpawner().apply(entity);
		}
	};
	struct GriddedItemSlotUISpawner :ecs::Recipe {
		GriddedItemSlotUISpawner(v2::Coord2 inv_loc):inventory_location(inv_loc) {

		}
		v2::Coord2 inventory_location;
		void apply(ecs::obj& entity) {
			ecs::obj item_decal = entity.spawn_child<ui::ui_image_spawner>(renderer::TexturePath("images\\blockholder.png", "item_decal_original"), geo::Box2d(v2::zerov, v2::unitv), 20);
			entity.ensure_component<ItemSlotDecal>(item_decal);
			FloatingItemSlotUiSpawner(inventory_center(inventory_location)).apply(entity);
		}
	};
	struct GriddedItemSlotSpawner:ecs::Recipe{
		GriddedItemSlotSpawner(v2::Coord2 inv_loc): inventory_location(inv_loc) {

		}
		v2::Coord2 inventory_location;
		void apply(ecs::obj& entity) {
			GriddedItemSlotUISpawner(inventory_location).apply(entity);
			ItemSlotSpawner().apply(entity);
		}
	};

}