
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
	struct UiEnabled :ecs::component {
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
			local_enabled = is_enabled;
		}
	};
	struct UiBounds :ecs::component {
		geo::Box2d local;
		UiBounds(geo::Box2d local_bounds) :local(local_bounds) {
		};
		v2::Vec2 local_center() const {
			return local.center;
		}
		v2::Vec2 local_scale() const {
			return local.scale;
		}

	};
	struct BaseUiNode :ecs::resource {
		ecs::obj base_node;
		BaseUiNode(ecs::obj base) :base_node(base) {

		}
	};
	struct ComputedStyle :ecs::component {
		size_t priority;
		geo::Box2d final_size;
		bool enabled;
		ComputedStyle(size_t precedence, geo::Box2d size, bool is_enabled) :final_size(size), priority(precedence), enabled(is_enabled) {

		}

	};
	struct ComputePrioritySystem :ecs::System {

		void run(ecs::Ecs& world) {
			auto query = ecs::View< ComputedStyle>(world);
			//disabled all styles allowing us to skip prossesing later on
			for (stn::TupleSet<ComputedStyle&> style:query) {
				style.get<ComputedStyle&>().enabled = false;
			}

			size_t current_assignment = 0;
			stn::stack<ecs::obj> ui_nodes;
			ui_nodes.push(world.get_resource<BaseUiNode>().base_node);

			//runs a dfs
			while (!ui_nodes.empty()) {
				ecs::obj next = ui_nodes.pop();
				stn::Option<ecs::obj> parent = ecs::parent(next);
				stn::Option<ComputedStyle&> prior_style = parent.map_member(&ecs::obj::get_component<ComputedStyle>);
				//we do not need to look at the prior style to get this becuse we only push in the case of the parent being enabled
				bool computed_enabled = next.get_component<UiEnabled>().local_enabled;
				//if we are not enabled their is no need to do write its properties and children out
				if (computed_enabled) {
					geo::Box2d local_size = next.get_component<UiBounds>().local;
					geo::Box2d computed_size = prior_style.member(&ComputedStyle::final_size)
						.unwrap_or(geo::Box2d(v2::zerov, v2::unitv)).transform(local_size);
					next.set_emplace_component<ComputedStyle>(current_assignment++, computed_size, computed_enabled);
					stn::array<ecs::obj> children;
					for (ecs::entity ent : ecs::HierarchyView(next).children_entities()) {
						if (world.has_component<UiPriority>(ent)) {
							children.push(ecs::obj(ent, world));
						}
					}
						children | stn::sort([](ecs::obj object)->size_t {return object.get_component<UiPriority>().priority; });
						children.reverse_in_place();
					//so children who are later are pushed last
					ui_nodes.push(children);
				}
			}
		}

	};



	struct InteractionState :ecs::component {
		bool hovered;
		bool right_clicked;
		bool left_clicked;
		InteractionState() :hovered(false), right_clicked(false), left_clicked(false) {
		};
	};



	struct UiInteractionSystem :ecs::System {
		void run(ecs::Ecs& world) {
			userinput::InputManager& man = world.get_resource<userinput::InputManager>();
			v2::Vec2 pos = man.mouse_position;
			bool has_clicked_left = man.left_mouse().pressed;
			bool has_clicked_right = man.right_mouse().pressed;
			ecs::View<ComputedStyle,InteractionState> bounds_view(world);
			for (auto&& [style, ui_interaction] : bounds_view) {
				bool cursor_touching = style.enabled&&style.final_size.contains(pos);
					ui_interaction.hovered = cursor_touching;
					ui_interaction.left_clicked = has_clicked_left&&ui_interaction.hovered;
					ui_interaction.right_clicked= has_clicked_right && ui_interaction.hovered;
			}
		}
	};
	struct UiSpawner {
		geo::Box2d bounds;
		size_t priority;
		UiSpawner(geo::Box2d box, size_t priority)
			:bounds(geo::Box2d(box.center, box.scale)), priority(priority){
		}
		static UiSpawner with_default_priority(geo::Box2d box) {
			return UiSpawner(box, 0);
		}
		static UiSpawner with_default_size(size_t priority) {
			return UiSpawner(geo::unit_box_2d,priority);
		}
		void apply(ecs::obj& object) const{
			object.set_emplace_component<UiBounds>(bounds);
			object.set_emplace_component<UiEnabled>();
			object.set_emplace_component<InteractionState>();
			object.set_emplace_component<UiPriority>(priority);
			object.set_emplace_component<ComputedStyle>(0, geo::Box2d(v2::zerov, v2::unitv), false);
			//for idempotency
			if (!object.has_component<ecs::Child>()) {
				object
					.world()
					.get_resource_opt<BaseUiNode>()
					.member(&BaseUiNode::base_node)
					.then([object](ecs::obj node) {node.add_child(object); });
			}
		}
	};
	struct UiPlugin {
		void operator()(Core::App& app) {
			app.emplace_system< UiInteractionSystem>();
			app.emplace_system< ComputePrioritySystem>();
			ecs::obj entity = ecs::spawn_emplaced<UiSpawner>(app.Ecs, geo::Box2d::origin_centered(v2::unitv), 2);
			app.emplace_resource<BaseUiNode>(entity);
		}

	};

}
