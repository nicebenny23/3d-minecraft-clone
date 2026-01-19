#include "Container.h"
#include "SlotUi.h"
#include "Container.h"
#include "slot_transactions.h"
#pragma once
namespace items {


	struct cursor_container:ecs::resource {
		ecs::obj primary_slot() {
			return container_object.get_component<container>()[container_index(0, 0)].get();
		}
		cursor_container(container& container):container_object(container.owner()) {
		}
		ecs::obj container_object;
	};

	struct cursor_swapper:ecs::System {
		void run(ecs::Ecs& world) {
			ecs::obj cursor_obj = world.get_resource<cursor_container>().unwrap().primary_slot();
			ElementSlot& cursor_slot=cursor_obj.get_component<ElementSlot>();
			cursor_obj.get_component<ui::ui_bounds>().local.center = world.ensure_resource<userinput::InputManager>().mouse_position;
			for (auto&& [item_decal,container_slot,interaction_state] : ecs::View<ItemDecal,ElementSlot,ui::InteractionState>(world)) {
				if (interaction_state.left_clicked) {
					if (container_slot.occupied()|| cursor_slot.occupied()) {
						swap_slot(cursor_slot, container_slot).plan(world).unwrap().apply(world);
						return;

					}	
				}
			}
		}

	};

	struct CursorContainerPlugin :Core::Plugin {
		void build(Core::App& app) {
		ecs::obj cursor_entity = ecs::spawn(app.Ecs, floating_container_recipe());
		app.emplace_system<cursor_swapper>();
		app.emplace_resource<cursor_container>(cursor_entity.get_component<container>());
		}
	};

};