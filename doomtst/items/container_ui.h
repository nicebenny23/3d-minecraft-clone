#include "Container.h"
#include "../renderer/ui_table.h"
#include "SlotUi.h"
#pragma once
namespace items{
	using slot_type = ecs::Constrained<ItemSlotDecal, RefrencedSlot>;
	struct ContainerDisplay:ecs::component {
		ContainerDisplay(ecs::Constrained<items::Container> object) :container_object(object), slots(){
		}
		ecs::Constrained<items::Container> container_object;
		stn::array<slot_type> slots;
		
		const Container& displayed_container() const {
			return container_object.get_component<Container>();
		}
		Container& displayed_container(){
			return container_object.get_component<Container>();
		}

		ui::TableBounds size() const {
			return displayed_container().size;
		}
		slot_type operator[](v2::UVec2 ind) {
			return slots[size().index(ind)];
		}
		const slot_type& operator[](v2::UVec2 ind) const {
			if (!size().contains(ind)) {
//				stn::throw_logic_error("index {} does not fit into size {}", ind.coord, size());
			}
			return slots[size().index(ind)];
		}
	};

	struct ContainerDisplayedRecipe {
		ecs::Constrained<items::Container> container_object;
		v2::Coord2 offset;
		void apply(ecs::obj& object) const {
			v2::UVec2 index = object.get_component<ui::TableEntry>().entry;
			ecs::Constrained< ElementSlot> slot_object(container_object.get<items::Container>()[index].object());
			object.apply_recipe(ItemSlotDisplaySpawner(slot_object, offset + v2::Coord2(index.x, index.y)));
			ecs::obj parent = ecs::parent(object).unwrap();
			parent.get_component<items::ContainerDisplay>().slots.push(slot_type(object));
		}
	};
	struct ContainerDisplayRecipe {
		v2::Coord2 offset;
		ecs::Constrained<items::Container> container_object;
		ContainerDisplayRecipe(v2::Coord2 position, ecs::Constrained<items::Container> object):offset(position),container_object(object){
			object.validate();
		}
		void apply(ecs::obj& object) const {
			ContainerDisplay& cont = object.add_component<ContainerDisplay>(container_object);
			
			ui::UiSpawner element_spawner = ui::UiSpawner::with_default_size(111);
			ui::UiTableRecipe< ContainerDisplayedRecipe>(ui::UiSpawner(geo::unit_box_2d,100),ContainerDisplayedRecipe{.container_object=container_object,.offset=offset},cont.displayed_container().size, element_spawner).apply(object);
			
		}
	};
}