#include "Container.h"
#include "SlotUi.h"
#pragma once
namespace items{
	struct ContainerDisplay:ecs::component {
		ContainerDisplay(ecs::Constrained<items::container> object) :container_object(object), slots(){
		}
		ecs::Constrained<items::container> container_object;
		stn::array<ecs::ConstrainedHandle<ItemSlotDecal,RefrencedSlot>> slots;
		const container& displayed_container() const {
			return container_object.get_component<container>();
		}
		container& displayed_container(){
			return container_object.get_component<container>();
		}
		v2::Coord2 size() const {
			return displayed_container().size;
		}
		ecs::ConstrainedHandle<ItemSlotDecal, RefrencedSlot>& operator[](container_index ind) {
			if (!ind.fits_in(size())) {
				stn::throw_logic_error("index {} does not fit into size {}", ind.coord, size());
			}
			return slots[ind.index_in(size())];
		}
		const ecs::ConstrainedHandle<ItemSlotDecal, RefrencedSlot>& operator[](container_index ind) const {
			if (!ind.fits_in(size())) {
				stn::throw_logic_error("index {} does not fit into size {}", ind.coord, size());
			}
			return slots[ind.index_in(size())];
		}
	};
	struct ContainerDisplayRecipe {
		v2::Coord2 offset;
		ecs::Constrained<items::container> container_object;
		ContainerDisplayRecipe(v2::Coord2 position, ecs::Constrained<items::container> object):offset(position),container_object(object){
			object.validate();
		}
		void apply(ecs::obj& object) const{
			ui::UiSpawner(geo::unit_box_2d, 1000).apply(object);
			ContainerDisplay& cont = object.add_component<ContainerDisplay>(container_object);
			v2::Coord2 size = cont.displayed_container().size;
			for (int y = 0; y < size.y; y++) {
				for (int x = 0; x < size.x; x++) {
					v2::Coord2 pos = v2::Coord2(x, y);
					v2::Coord2 display_pos = offset + pos;
					ecs::Constrained< ElementSlot> slot_at(cont.displayed_container()[pos].object());
					cont.slots.emplace(object.spawn_child<ItemSlotDisplaySpawner>(slot_at, display_pos));
				}
			}
		}
	};
}