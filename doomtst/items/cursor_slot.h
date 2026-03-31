#include "Container.h"
#include "SlotUi.h"
#include "Container.h"
#include "slot_transactions.h"
#include "menu.h"
#include "container_ui.h"
#pragma once
namespace items {


	struct cursor_container :ecs::resource {
		ecs::obj primary_slot() {
			return container_object;
		}
		cursor_container(ecs::Constrained<ElementSlot> container,ecs::obj cursor_display)
		:container_object(container.object()),display(cursor_display) {

		}
		ecs::obj display;
		ecs::obj container_object;
	};

	struct cursor_swapper :ecs::System {
		void run(ecs::Ecs& world) {
			if (world.get_resource<ui::MenuState>().no_menu_open()) {
				return;
			}
			ecs::obj cursor_obj = world.get_resource<cursor_container>().primary_slot();
			ElementSlot& cursor_slot = cursor_obj.get_component<ElementSlot>();
			for (auto&& [interaction_state, container_slot,decl] : ecs::View<ui::InteractionState,items::RefrencedSlot, items::ItemSlotDecal>(world)) {
				if (!interaction_state.left_clicked) {
					continue;
				}
				if (container_slot.displayed().empty() && cursor_slot.empty()) {
					continue;
				}
				if (container_slot.displayed().can_interact(cursor_slot)) {

					if (cursor_slot.occupied()) {
						auto cursor_slot_tranfer = transfer_slot_some(container_slot.displayed(), cursor_slot);
						if (cursor_slot_tranfer) {
							cursor_slot_tranfer.unwrap().apply(world);
						}
					}
					else {
						auto slot_cursor_tranfer = transfer_slot_some(cursor_slot, container_slot.displayed());
						if (slot_cursor_tranfer) {
							slot_cursor_tranfer.unwrap().apply(world);
						}
					}
				}
				else {
					swap_slot(cursor_slot, container_slot.displayed()).apply(world);

				}
				return;
			}

		}
	};
	struct cursor_spreader :ecs::System {
		void run(ecs::Ecs& world) {
			if (world.get_resource<ui::MenuState>().no_menu_open()) {
				return;
			}
			ecs::obj cursor_display = world.get_resource<cursor_container>().display;
			ElementSlot& cursor_slot = world.get_resource<cursor_container>().primary_slot().get_component<ElementSlot>();
			cursor_display.get_component<ui::UiBounds>().local.center = world.get_resource<userinput::InputManager>().mouse_position;
			debug(cursor_display.get_component<ui::ComputedStyle>().enabled);
			for (auto&& [interaction_state, container_slot,decl] : ecs::View< ui::InteractionState, items::RefrencedSlot,   items::ItemSlotDecal>(world)) {
				if (!interaction_state.right_clicked) {
					continue;
				}
				if (cursor_slot.empty() && container_slot.displayed().empty()) {
					continue;
				}
				//implies container_slot is nonempty;
				if (cursor_slot.empty()) {

					auto give_cursor_half = give_slot_some(container_slot.displayed(), cursor_slot, container_slot.displayed().entry().unwrap().count / 2);
					if (give_cursor_half) {
						give_cursor_half.unwrap().apply(world);
					}

				}
				else {
					auto give_slot_one = give_slot_some(cursor_slot, container_slot.displayed(), 1);
					if (give_slot_one) {
						give_slot_one.unwrap().apply(world);
					}
				}
				return;

			}
		}
	};
	struct cursor_highlighter :ecs::System {
		void run(ecs::Ecs& world) {
			world.get_resource<cursor_container>().display.get_component<ui::UiEnabled>().enable();
			ecs::obj cursor_obj = world.get_resource<cursor_container>().primary_slot();
			ui::ComputedStyle & style = ecs::parent(world.get_resource<cursor_container>().display)
			.unwrap()
			.get_component<ui::ComputedStyle>();

			ElementSlot& cursor_slot = cursor_obj.get_component<ElementSlot>();
			for (auto [item_decal, interaction_state] : ecs::View< ItemSlotDecal, ui::InteractionState>(world)) {
				if (interaction_state.hovered) {
					if (world.get_resource<ui::MenuState>().menu_open()) {
						item_decal.set_decal(renderer::TexturePath("images\\importantblockholder.png", "important_block_holder"));
					}
				}
				else {
					item_decal.reset_decal();
				}
			}
		}
	};



	struct CursorContainerPlugin :Core::Plugin {
		void build(Core::App& app) {
			ecs::obj cursor_entity = ecs::spawn(app.Ecs, items::ItemSlotSpawner());
			ecs::obj cursor_display = ecs::spawn(app.Ecs, items::ClearItemSlotSpawner(ecs::Constrained<items::ElementSlot>(cursor_entity)));
				
			app.emplace_system<cursor_swapper>();
			app.emplace_system<cursor_spreader>();
			app.emplace_system<cursor_highlighter>();
			app.emplace_resource<cursor_container>(cursor_entity, cursor_display);
		}
	};

};