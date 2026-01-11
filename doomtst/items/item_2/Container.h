#include "ItemSlot.h"
#pragma once
namespace items {
	struct container_tag{ };
	using container_id = stn::typed_id<container_tag>;
	struct container :ecs::component{
		stn::array <ecs::object_handle> slots;
		v2::Coord2 size;
		container_id cont_id;
		container(v2::Coord2 Size, container_id id) :size(Size), cont_id(id) {
		}

		void start() {
			for (size_t x = 0; x < size.x; x++) {
				for (size_t y = 0; y < size.y; y++) {
					slots.push(ecs::object_handle(ecs::spawn(world(), ItemSlotSpawner(container_index(x,y)))));
				}
			}
		}
		

	};
	struct AddToContainer:tr

}