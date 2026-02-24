#include "ItemSlot.h"
#include "SlotUi.h"
#include "item_type.h"
#pragma once
namespace items {
	struct container_id_tag {
	};
	using container_id =stn::typed_id<container_id_tag >;
	
	struct container :ecs::component {
		stn::array <ecs::object_handle> slots;
		v2::Coord2 size;
		container_id id;
		container(v2::Coord2 Size, container_id cont_id) :size(Size),id(cont_id){
		}
		void start() {

		}
		ecs::object_handle& operator[](container_index ind) {
			if (!ind.fits_in(size)) {
				stn::throw_logic_error("index {} does not fit into size {}", ind.coord, size);
			}
			return slots[ind.index_in(size)];
		}
		const item_types& types() const{
			return world().get_resource<item_types>();
		}
		const ecs::object_handle& operator[](container_index ind) const {
			if (!ind.fits_in(size)) {
				stn::throw_logic_error("index {} does not fit into size {}", ind.coord, size);
			}
			return slots[ind.index_in(size)];
		}
		const bool contains(container_index ind) const {
			return ind.fits_in(size);
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
		v2::Coord2 offset;
		container_recipe(v2::Coord2 off):offset(off) {

		}
		void apply(ecs::obj& ent) const{
			WorldContainers& containers=ent.world().ensure_resource<WorldContainers>();
			container_id id = containers.gen_next();
			container& cont = ent.add_component< container>(offset, id);
			containers.containers.emplace(ent);
			for (int y = 0; y < offset.y; y++) {
				for (int x = 0; x < offset.x; x++) {
					cont.slots.emplace<ecs::object_handle>(ecs::spawn(ent.world(), ItemSlotSpawner()));
					ent.add_child(cont.slots.last().get());
				}
			}
		}
	};
}