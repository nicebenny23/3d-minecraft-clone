#include "ItemSlot.h"
#include "SlotUi.h"
#include "../renderer/ui_table.h"
#include "item_type.h"
#pragma once
namespace items {
	struct container_id_tag {
	};
	using container_id =stn::typed_id<container_id_tag >;
	using container_element = ecs::ConstrainedHandle<ElementSlot>;
	struct container :ecs::component {
		stn::array <container_element> slots;
		ui::TableBounds size;
		container_id id;
		container(ui::TableBounds Size, container_id cont_id) :size(Size),id(cont_id){
		}
		void start() {

		}
		container_element& operator[](v2::UVec2 ind) {
			return slots[size.index(ind)];
		}
		const ItemTypes& types() const{
			return world().get_resource<ItemTypes>();
		}
		const container_element& operator[](v2::UVec2 ind) const {
			return slots[size.index(ind)];
		}
		const bool contains(v2::UVec2 ind) const {
			return size.contains(ind);
		}
		using iterator = decltype(slots)::iterator;
		iterator begin() {
			return slots.begin();
		}
		iterator end() {
			return slots.end();
		}
		using const_iterator = decltype(slots)::const_iterator;
		const_iterator begin() const {
			return slots.begin();
		}
		const_iterator end() const {
			return slots.end();
		}

	};
	//because i cannot get serilization to work
	struct WorldContainers :ecs::resource {
		stn::array<ecs::Constrained<container>> containers;
		container_id gen_next() const{
			return container_id(containers.length());
		}
		ecs::Constrained<container>& operator[](container_id index) {
			return containers[index.id];
		}
	};

	struct container_recipe {
		ui::TableBounds offset;
		container_recipe(ui::TableBounds off):offset(off) {

		}
		void apply(ecs::obj& ent) const{
			WorldContainers& containers=ent.world().ensure_resource<WorldContainers>();
			container_id id = containers.gen_next();
			container& cont = ent.add_component<container>(offset, id);
			containers.containers.emplace(ent);
			for (size_t i = 0; i < offset.entries();i++) {
					cont.slots.emplace<items::container_element>(ecs::spawn(ent.world(), ItemSlotSpawner()));
					ent.add_child(cont.slots.last().object());
			}
		}
	};
}