#pragma once
#include "game_object.h"
namespace ecs {

	struct HierarchyView {
		ecs::obj entity;

		HierarchyView(const ecs::obj e) : entity(e) {
		}
		ecs::obj operator*() {
			return entity;
		}
		const ecs::obj operator*() const {
			return entity;
		}

		ecs::obj* operator->() {
			return &entity;
		}
		const ecs::obj* operator->() const {
			return &entity;
		}

		bool has_children() {
			Parent* prnt = entity.get_component_ptr<Parent>();
			return prnt && !prnt->empty();
		}
		bool has_parent() {
			return entity.has_component<Child>();
		}
		stn::Option<HierarchyView> parent() {
			return entity.get_component_opt<Child>().map([&](Child& child) {return HierarchyView(ecs::obj(child.parent(), child.world())); });
		}
		stn::array<HierarchyView> children() {
			if (!entity.has_component<Parent>()) return {};

			stn::array<HierarchyView> view;
			for (const auto& child : entity.get_component<Parent>().span()) {
				view.push(HierarchyView(ecs::obj(child, entity.world())));
			}
			return view;
		}
		stn::span<ecs::entity> children_obj() {
			if (!entity.has_component<Parent>()) return {};

			return entity.get_component<Parent>().span();

		}
	};
	inline HierarchyView view(ecs::obj object) {

		return HierarchyView(object);
	}
	inline bool has_children(ecs::obj entity) {
		return view(entity).has_children();
	}
	inline bool has_parent(ecs::obj entity) {
		return view(entity).has_parent();
	}
	inline stn::Option<ecs::obj> parent(ecs::obj entity) {
		return view(entity).parent().map(std::function([](HierarchyView view) {return view.entity; }));
	}
	inline stn::span<ecs::entity> children(ecs::obj entity) {
		return view(entity).children_obj();
	}
}