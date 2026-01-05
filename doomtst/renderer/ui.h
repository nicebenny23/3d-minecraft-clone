
#pragma once
#include "../util/owner.h"
#include "../util/dynamicarray.h"
#include "../debugger/debug.h"
#include "../game/Core.h"
#include "../game/ecs/relationship.h"
#include "../game/ecs/query.h"
#include "../game/ecs/spawner.h"
#define uisize 400
using namespace stn;
using namespace Cptr;
namespace ui {

	struct ui_handle;

	struct ui_bounds :ecs::component {
		geometry::Box2d local;
		ui_bounds(geometry::Box2d local_bounds) :local(local_bounds) {
		};
		v2::Vec2 local_center() const {
			return local.center;
		}
		v2::Vec2 local_scale() const {
			return local.scale;
		}
		geometry::Box2d global() const {
			if (ecs::has_parent(owner())) {
				geometry::Box2d owner_box = ecs::parent(owner()).unwrap().get_component<ui_bounds>().global();
				v2::Vec2 scale = owner_box.scale * local_scale();
				return geometry::Box2d(owner_box.center + scale * local_center(), scale);
			}
			else {
				return local;
			}
		}
	};

	struct ui_enabled :ecs::component {
		bool enabled() {
			if (ecs::has_parent(owner())) {
				return local_enabled && ecs::parent(owner()).unwrap().get_component<ui_enabled>().enabled();
			}
			return local_enabled;
		}
		ui_enabled() :local_enabled(true) {
		};
		bool local_enabled;
		void disable() {
			local_enabled = false;
		}
		void enable() {
			local_enabled = true;
		}
	};
	struct InteractionState :ecs::component {
		bool hovered;
		bool right_clicked;
		bool left_clicked;
		InteractionState() = default;
	};




	struct ui_interaction_system :ecs::System {
		void run(ecs::Ecs& world) {
			v2::Vec2 pos = CtxName::ctx.Inp->mousepos;
			bool has_clicked_left = CtxName::ctx.Inp->mouseleft().pressed;
			bool has_clicked_right = CtxName::ctx.Inp->mouseright().pressed;

			ecs::View<ui_enabled, ui_bounds, InteractionState> bounds_view(world);
			for (auto&& [enabled, bounds, ui_interaction] : bounds_view) {
				bool cursor_touching = bounds.global().contains(pos) && enabled.enabled();
				ui_interaction.left_clicked = cursor_touching && has_clicked_left;
				ui_interaction.right_clicked = cursor_touching && has_clicked_right;
				ui_interaction.hovered = cursor_touching;
			}
		}
	};

	struct ui_spawner {
		geometry::Box2d bounds;

		ui_spawner(geometry::Box2d bounds) :bounds(bounds) {

		}
		void apply(ecs::obj object) {
			object.add_component<ui_bounds>(bounds);
			object.add_component<ui_enabled>();
			object.add_component<InteractionState>();
		}
	};
	//ecs object specilized for ui
	struct ui_handle {
		ui_handle(ecs::Ecs& world, geometry::Box2d bounds) :object(ecs::spawn_emplaced<ui_spawner>(world, bounds)) {

		}
		ecs::obj object;
		InteractionState state() {
			return object.get_component<InteractionState>();
		}
		bool enabled() {
			return object.get_component<ui_enabled>().enabled();
		}
		void enable() {
			object.get_component<ui_enabled>().enable();
		}
		void disable() {
			object.get_component<ui_enabled>().disable();
		}
		Box2d bounds() const {
			return object.get_component<ui_bounds>().global();
		}
		void set_bounds(Box2d bounds) {
			object.get_component<ui_bounds>().local = bounds;
		}
		void set_center(v2::Vec2 center) {
			object.get_component<ui_bounds>().local.center = center;
		}


		void add_child(const ui_handle& child) {
			object.ensure_component<ecs::Parent>().add_child(child.object);
		}
		template<ecs::ComponentType T>
		T& get_component() {
			return object.get_component<T>();
		}

	};
}
