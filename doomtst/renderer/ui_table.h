#include "ui.h"
#include "../math/mathutil.h"
#pragma once
namespace ui {
	struct TableBounds {
		TableBounds(size_t x, size_t y) :table(x,y) {

		}
		TableBounds():table(0,0) {

		}
		TableBounds(v2::UVec2 size) :table(size) {

		}
		v2::UVec2 table;
		
		size_t entries() const {
			return table.x * table.y;
		}
		bool empty() const{
			return entries() == 0;
		}
		bool contains(v2::UVec2 value) const {
			return value.x < table.x && value.y < table.y;
		}
		bool contains(TableBounds value) const {
			return contains(value.table);
		}
		size_t index(v2::UVec2 value) const {
			if (!contains(value)) {
				stn::throw_logic_error("index was not contained");
			}
			return value.y * table.x + value.x;
		}

		bool operator==(const TableBounds& other) const = default;
		bool operator!=(const TableBounds& other) const = default;
		struct iterator {
			iterator(v2::UVec2 curr,size_t x_end) :x_end_bounds(x_end), current(curr) {

			}
			v2::UVec2 operator*() const {
				return current;
			}
			iterator& operator++() {
				if (++current.x==x_end_bounds) {
					current.x = 0;
					current.y++;
				}
				return *this;
			}
			bool operator==(const iterator& other) const = default;
			bool operator!=(const iterator& other) const = default;
		private:

			size_t x_end_bounds;
			v2::UVec2 current;
		};
		
		iterator end() const {
			return iterator(v2::UVec2(0,table.y),table.x);
		}
		iterator begin() const {
			return iterator(v2::UVec2(0, 0), table.x);
		}
	};

	struct TableEntry:ecs::component {
		TableEntry(v2::UVec2 value):entry(value) {

		}
		v2::UVec2 entry;
	};


	template<ecs::RecipeType T>
	struct UiTableRecipe {
		UiTableRecipe(UiSpawner ui_spawner,T recipe, TableBounds size, UiSpawner element_spawner):size(size),recipe(recipe), element_spawner(element_spawner), ui_spawner(ui_spawner){

		}
		
		UiSpawner element_spawner;
		UiSpawner ui_spawner;
		TableBounds size;
		T recipe;
		void apply(ecs::obj& object) const {
			ui_spawner.apply(object);
			if (size.empty()) {
				return;
			}
			v2::Vec2 inv_size = v2::Vec2(1.0 / size.table.x, 1.0 / size.table.y);
			for (v2::UVec2 pos : size) {
				v2::Vec2 point = v2::Vec2(math::centered_unit_bounds.lerp((.5f+pos.x) * inv_size.x), math::centered_unit_bounds.lerp((.5f + pos.y) * inv_size.y));
				geo::Box2d child_box=geo::Box2d(point, inv_size).transform(element_spawner.bounds);
				ecs::obj child= object.spawn_child<ui::UiSpawner>(child_box, element_spawner.priority);
				child.add_component<TableEntry>(pos);
				ecs::obj parent = ecs::parent(child).unwrap();
				child.apply_recipe(recipe);
			}


		}
	};

}
namespace std {


	template <>
	struct formatter<ui::TableBounds> : formatter<string> {
		template <typename FormatContext>
		auto format(const ui::TableBounds& v, FormatContext& ctx) const {
			return format_to(ctx.out(),"{}",v.table);
		}
	};
}