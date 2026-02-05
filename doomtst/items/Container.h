#include "ItemSlot.h"
#include "SlotUi.h"
#pragma once
namespace items {
	struct container :ecs::component {
		stn::array <ecs::object_handle> slots;
		v2::Coord2 size;
		container(v2::Coord2 Size) :size(Size){
		}
		void start() {
		
		}
		ecs::object_handle& operator[](container_index ind) {
			if (!ind.fits_in(size)) {
				stn::throw_logic_error("index {} does not fit into size {}", ind.coord, size);
			}
			return slots[ind.x() * size.y + ind.y()];
		}
		const ecs::object_handle& operator[](container_index ind) const {
			if (!ind.fits_in(size)) {
				stn::throw_logic_error("index {} does not fit into size {}", ind.coord, size);
			}
			return slots[ind.x() * size.y + ind.y()];
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
	struct container_recipe :ecs::Recipe {
		v2::Coord2 min_position;
		v2::Coord2 offset;
		container_recipe(v2::Coord2 min, v2::Coord2 off):min_position(min),offset(off){

		}
		void apply(ecs::obj& ent) {
			ui::ui_spawner(geo::Box2d(v2::zerov, v2::unitv), 1000).apply(ent);
			
			container& cont= ent.add_component< container>(offset);

			for (int x = 0; x < offset.x; x++) {
				for (int y = 0; y < offset.y; y++) {
					v2::Coord2 pos = v2::Coord2(x, y);
					cont.slots.emplace<ecs::object_handle>(std::move(ecs::spawn(ent.world(), GriddedItemSlotSpawner(pos + min_position))));
					ent.add_child(cont.slots.last().get());
				}
			}
		}
	};
	struct container_ui_recipe :ecs::Recipe {
		v2::Coord2 min_position;
		v2::Coord2 offset;
		container_ui_recipe(v2::Coord2 min, v2::Coord2 off) :min_position(min), offset(off) {

		}
		void apply(ecs::obj& ent) {
			ui::ui_spawner(geo::Box2d(v2::zerov, v2::unitv), 1000).apply(ent);

			container& cont = ent.add_component< container>(offset);

			for (int x = 0; x < offset.x; x++) {
				for (int y = 0; y < offset.y; y++) {
					v2::Coord2 pos = v2::Coord2(x, y);
					cont.slots.emplace<ecs::object_handle>(std::move(ecs::spawn(ent.world(), GriddedItemSlotUISpawner(pos + min_position))));
					ent.add_child(cont.slots.last().get());
				}
			}
		}
	};

	struct floating_container_recipe :ecs::Recipe {
		
		floating_container_recipe(){

		}
		void apply(ecs::obj& ent) {
			ui::ui_spawner(geo::Box2d(v2::zerov, v2::unitv), 10000).apply(ent);
			container& cont = ent.add_component< container>(v2::Coord2(1,1));
			cont.slots.emplace<ecs::object_handle>(std::move(ecs::spawn(ent.world(), FloatingItemSlotSpawner(v2::zerov))));
			ent.add_child(cont.slots.last().get());
		}
	};

}