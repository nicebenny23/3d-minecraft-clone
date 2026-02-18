#include "ItemSlot.h"
#include "SlotUi.h"
#pragma once
namespace items {
	struct container :ecs::component {
		stn::array <ecs::object_handle> slots;
		v2::Coord2 size;
		container(v2::Coord2 Size) :size(Size) {
		}
		void start() {

		}
		ecs::object_handle& operator[](container_index ind) {
			if (!ind.fits_in(size)) {
				stn::throw_logic_error("index {} does not fit into size {}", ind.coord, size);
			}
			return slots[ind.index_in(size)];
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
	struct container_recipe {
		v2::Coord2 offset;
		container_recipe(v2::Coord2 off):offset(off) {

		}
		void apply(ecs::obj& ent) {
			container& cont = ent.add_component< container>(offset);
			for (int y = 0; y < offset.y; y++) {
				for (int x = 0; x < offset.x; x++) {
					cont.slots.emplace<ecs::object_handle>(ecs::spawn(ent.world(), ItemSlotSpawner()));
					ent.add_child(cont.slots.last().get());
				}
			}
		}
	};
}