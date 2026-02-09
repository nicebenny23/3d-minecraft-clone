#include "Container.h"
#include "SlotUi.h"
#pragma once
namespace items{
	struct ContainerDisplay:ecs::component {
		ContainerDisplay(ecs::obj object) :container_object(object) {
		}
		ecs::obj container_object;
		const container& displayed_container() const {
			return container_object.get_component<container>();
		}
		container& displayed_container(){
			return container_object.get_component<container>();
		}
	};
	struct ContainerDisplayRecipe :ecs::Recipe {
		v2::Coord2 offset;
		ecs::obj container_object;
		ContainerDisplayRecipe(v2::Coord2 position, ecs::obj object):offset(position),container_object(object){
		}
		void apply(ecs::obj& object) {
			ui::UiSpawner(geo::Box2d(v2::zerov, v2::unitv), 1000).apply(object);
			ContainerDisplay& cont = object.add_component<ContainerDisplay>(container_object);
			v2::Coord2 size = cont.displayed_container().size;
			for (int y = 0; y < size.y; y++) {
				for (int x = 0; x < size.x; x++) {
					v2::Coord2 pos = v2::Coord2(x, y);
					v2::Coord2 display_pos = offset + pos;
					ecs::obj slot_at = cont.displayed_container()[pos].get();
					ecs::obj idk=object.spawn_child<ItemSlotDispaySpawner>(slot_at, display_pos);
				}
			}
		}
	};
}