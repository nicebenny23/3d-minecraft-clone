#include "../game/ecs/filtered_object.h"
#include "ItemSlot.h"
#include "../math/geometry.h"
#pragma once
namespace items {
	inline constexpr v2::Coord2 grid_size = v2::Coord2(30, 30);
	inline geo::Box2d inventory_transform_floating(v2::Vec2 center) {
		v2::Vec2 scale = v2::Vec2(1.0f / grid_size.x, 1.0f / grid_size.y);
		return geo::Box2d(center, scale);
	}
	inline v2::Vec2 inventory_center(v2::Coord2 location) {
		float xval = (2 * location.x + 1.0f) / float(grid_size.x);
		float yval = (2 * location.y + 1.0f) / float(grid_size.y);
		return v2::Vec2(xval, yval)/2;
	}


	struct ItemSlotDecal :ecs::component {
		ItemSlotDecal(ecs::obj decal_component) :item_decal_object(decal_component) {
		}
		void set_decal(const renderer::TexturePath& Path) {
			item_decal_object.get<ui::Image>().set_image(Path);
		}
		void reset_decal() {
			renderer::TexturePath path = renderer::TexturePath("images\\blockholder.png");
				set_decal(path);			
		}
		ecs::Constrained<ui::Image> item_decal_object;
	};

	
}