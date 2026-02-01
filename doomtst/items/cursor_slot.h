#include "Container.h"
#include "SlotUi.h"
#include "Container.h"
#include "slot_transactions.h"
#pragma once
namespace items {


	struct cursor_container :ecs::resource {
		ecs::obj primary_slot() {
			return container_object.get_component<container>()[container_index(0, 0)].get();
		}
		cursor_container(container& container) :container_object(container.owner()) {
		}
		ecs::obj container_object;
		bool take_one_mode;
	};

	struct cursor_swapper :ecs::System {
		void run(ecs::Ecs& world) {

			ecs::obj cursor_obj = world.get_resource<cursor_container>().unwrap().primary_slot();
			ElementSlot& cursor_slot = cursor_obj.get_component<ElementSlot>();
			cursor_obj.get_component<ui::UiBounds>().local.center = world.ensure_resource<userinput::InputManager>().mouse_position;
			for (auto&& [item_decal, interaction_state, container_slot] : ecs::View<ItemSlotDecal, ui::InteractionState, items::ElementSlot>(world)) {
				if (!interaction_state.left_clicked) {
					continue;
				}
				if (container_slot.empty() && cursor_slot.empty()) {
					continue;
				}
				if (container_slot.can_interact(cursor_slot)) {

					if (cursor_slot.occupied()) {
						auto cursor_slot_tranfer = transfer_slot_some(container_slot, cursor_slot);
						if (cursor_slot_tranfer) {
							cursor_slot_tranfer.unwrap().apply(world);
						}
					}
					else {
						auto slot_cursor_tranfer = transfer_slot_some(cursor_slot, container_slot);
						if (slot_cursor_tranfer) {
							slot_cursor_tranfer.unwrap().apply(world);
						}
					}
				}
				else {
					swap_slot(cursor_slot, container_slot).apply(world);

				}
				return;
			}

		}
	};
	struct cursor_spreader :ecs::System {
		void run(ecs::Ecs& world) {

			ecs::obj cursor_obj = world.get_resource<cursor_container>().unwrap().primary_slot();
			ElementSlot& cursor_slot = cursor_obj.get_component<ElementSlot>();
			cursor_obj.get_component<ui::UiBounds>().local.center = world.ensure_resource<userinput::InputManager>().mouse_position;
			for (auto&& [item_decal, interaction_state, container_slot] : ecs::View<ItemSlotDecal, ui::InteractionState, items::ElementSlot>(world)) {
				if (!interaction_state.right_clicked) {
					continue;
				}
				if (cursor_slot.empty() && container_slot.empty()) {
					continue;
				}
				//implies container_slot is nonempty;
				if (cursor_slot.empty()) {

					auto give_cursor_half = give_slot_some(container_slot, cursor_slot, container_slot.entry().unwrap().count/2);
					if (give_cursor_half) {
						give_cursor_half.unwrap().apply(world);
					}

				}
				else {
					auto give_slot_one = give_slot_some(cursor_slot, container_slot, 1);
					if (give_slot_one) {
						give_slot_one.unwrap().apply(world);
					}
				}
				return;

			}
		}
	};



	struct CursorContainerPlugin :Core::Plugin {
		void build(Core::App& app) {
			ecs::obj cursor_entity = ecs::spawn(app.Ecs, floating_container_recipe());
			app.emplace_system<cursor_swapper>();
			app.emplace_system<cursor_spreader>();

			app.emplace_resource<cursor_container>(cursor_entity.get_component<container>());
		}
	};

};