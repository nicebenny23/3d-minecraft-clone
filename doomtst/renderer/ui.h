
#pragma once
#include "../util/dynamicarray.h"
#include "../debugger/debug.h"
#include "../game/Core.h"
#include "../game/ecs/relationship.h"
#include "../game/ecs/query.h"
#include "../game/ecs/spawner.h"
#include "../game/ecs/traversal.h"
#include "../util/stack.h"
#include "../math/geometry.h"
#include "../util/userinput.h"
namespace ui {

	struct UiPriority :ecs::component {
		size_t priority;
		UiPriority(size_t precedence) :priority(precedence) {

		}
		void set_priority(size_t precedence) {
			priority = precedence;
		}
	};
	struct ComputedStyle :ecs::component {
		size_t priority;
		ComputedStyle(size_t precedence) :priority(precedence) {

		}
	};


	struct BaseUiNode :ecs::resource {
		ecs::obj base_node;
		BaseUiNode(ecs::obj base) :base_node(base) {

		}
	};
	struct ComputePrioritySystem :ecs::System {

		void run(ecs::Ecs& world) {
			//runs a bfs
			size_t current_assignment = 0;
			stn::stack<ecs::obj> ui_nodes;
			ui_nodes.push(world.get_resource<BaseUiNode>().base_node);
			while (!ui_nodes.empty()) {
				ecs::obj next = ui_nodes.pop();
				next.set_emplace_component<ComputedStyle>(current_assignment).priority = current_assignment++;
				stn::array<ecs::obj> children;
				for (ecs::entity ent : ecs::HierarchyView(next).children_entities()) {
					if (world.has_component<UiPriority>(ent)) {
						children.push(ecs::obj(ent, world));
					}
				}
				children | stn::sort([](ecs::obj object)->int {return object.get_component<UiPriority>().priority; });
				//so children who are later are pushed last
				children.reverse_in_place();
				ui_nodes.push(children);
			}
		}

	};

	struct UiBounds :ecs::component {
		bool uses_global_bounds;
		geo::Box2d local;
		UiBounds(geo::Box2d local_bounds, bool global_bounds) :local(local_bounds),uses_global_bounds(global_bounds){
		};
		v2::Vec2 local_center() const {
			return local.center;
		}
		v2::Vec2 local_scale() const {
			return local.scale;
		}
		geo::Box2d global() const {
			if (ecs::has_parent(owner())&& !uses_global_bounds) {
				geo::Box2d owner_box = ecs::parent(owner()).unwrap().get_component_opt<UiBounds>()
				.expect("all parented ui elements must have a parent ui element")
				.global();
				v2::Vec2 owner_scale = owner_box.scale;
				return geo::Box2d(owner_box.center + owner_scale * local_center(), owner_scale * local_scale());
			}
			else {
				return local;
			}
		}
		v2::Vec2 center() const {
			return global().center;
		}

		v2::Vec2 scale() const {
			return global().scale;
		}
	};

	struct SetUiEnabled {
		SetUiEnabled(ecs::entity ent, bool enabled) :ent(ent), should_enable(enabled) {

		}
		bool should_enable;
		ecs::entity ent;
	};

	struct UiEnabled :ecs::component {
		bool enabled() {
			if (ecs::has_parent(owner())) {
				return local_enabled && ecs::parent(owner()).unwrap().get_component<UiEnabled>().enabled();
			}
			return local_enabled;
		}
		UiEnabled() :local_enabled(true) {
		};
		bool local_enabled;
		void disable() {

			set_enabled(false);
		}
		void enable() {
			set_enabled(true);
		}
		void set_enabled(bool is_enabled) {
			world().write_command(SetUiEnabled(owner().inner(), is_enabled));
		}
		void set_enable_state(bool enabled) {
			local_enabled = enabled;
		}
	};




	struct InteractionState :ecs::component {
		bool hovered;
		bool right_clicked;
		bool left_clicked;
		InteractionState() :hovered(false), right_clicked(false), left_clicked(false) {
		};
	};



	struct UiEnablerSystem :ecs::System {
		void run(ecs::Ecs& world) {
			for (SetUiEnabled cmd : world.read_commands<SetUiEnabled>()) {
				if (world.contains(cmd.ent)) {
					world.get_component<UiEnabled>(cmd.ent).set_enable_state(cmd.should_enable);
				}
			}


		}
	};
	struct UiInteractionSystem :ecs::System {
		void run(ecs::Ecs& world) {
			userinput::InputManager& man = world.get_resource<userinput::InputManager>();
			v2::Vec2 pos = man.mouse_position;
			bool has_clicked_left = man.left_mouse().pressed;
			bool has_clicked_right = man.right_mouse().pressed;
			if (has_clicked_left) {
				int l = 3;
			}
			ecs::View<UiEnabled, UiBounds, InteractionState> bounds_view(world);
			for (auto&& [enabled, bounds, ui_interaction] : bounds_view) {
				bool cursor_touching = bounds.global().contains(pos) && enabled.enabled();
				if (cursor_touching) {
					ui_interaction.left_clicked = has_clicked_left;
					ui_interaction.right_clicked = has_clicked_right;

				}
				ui_interaction.hovered = cursor_touching;
			}
		}
	};
	struct UiSpawner :ecs::Recipe {
		geo::Box2d bounds;
		size_t priority;
		bool bounds_type;
		UiSpawner(geo::Box2d box, size_t priority,bool global_bounds=false) 
		:bounds(geo::Box2d(box.center, box.scale)), priority(priority), bounds_type(global_bounds){
		}
		void apply(ecs::obj& object) {
			object.add_component<UiBounds>(bounds, bounds_type);
			object.add_component<UiEnabled>();
			object.add_component<InteractionState>();
			object.add_component<UiPriority>(priority);
			object.add_component<ComputedStyle>(0);
				
			object
			.world()
			.get_resource_opt<BaseUiNode>()
			.member(&BaseUiNode::base_node)
			.then([object](ecs::obj node) {node.add_child(object); });
	
		}
	};
	struct UiPlugin :Core::Plugin {
		void build(Core::App& app) {
			app.emplace_system< UiEnablerSystem>();
			app.emplace_system< UiInteractionSystem>();
			app.emplace_system< ComputePrioritySystem>();
			ecs::obj entity = ecs::spawn_emplaced<UiSpawner>(app.Ecs, geo::Box2d::origin_centered(v2::unitv), 2);
			app.emplace_resource<BaseUiNode>(entity);
		}

	};


}
