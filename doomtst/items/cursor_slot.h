#include "Container.h"
#include "SlotUi.h"
#include "Container.h"
#include "slot_transactions.h"
#include "menu.h"
#include "container_ui.h"
#pragma once
namespace items {


	struct cursor_container :ecs::resource {
		ecs::Constrained<ElementSlot> primary_slot() {
			return container_object;
		}
		cursor_container(ecs::Constrained<ElementSlot> container,ecs::obj cursor_display)
		:container_object(container),display(cursor_display) {
		}
		ecs::obj display;
		ecs::Constrained<ElementSlot>  container_object;
	};

	struct cursor_swapper :ecs::System {
		void run(ecs::Ecs& world) {
			if (world.get_resource<ui::MenuState>().no_menu_open()) {
				return;
			}
			ecs::Constrained<ElementSlot> cursor_object = world.get_resource<cursor_container>().primary_slot();
			ElementSlot& cursor_slot = cursor_object.get<ElementSlot>();
			for (auto&& [interaction_state, container_slot,decl] : ecs::View<ui::InteractionState,items::RefrencedSlot, items::ItemSlotDecal>(world)) {
				if (!interaction_state.left_clicked) {
					continue;
				}
				ElementSlot& cont_slot = container_slot.displayed().get<ElementSlot>();
				if (cont_slot.empty() && cursor_slot.element().is_none()) {
					continue;
				}
				//hack because idk how to make items durbility work
				if (!cont_slot.empty()&&items::apply_plan(TransferSlotsRequest{ .from_slot = cursor_object,.to_slot = container_slot.displayed(), .count = items::max_transfer })) {

				}
				else {
					swap_slot_plans(cursor_object, container_slot.displayed());
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
			ecs::Constrained<ElementSlot> cursor_slot = world.get_resource<cursor_container>().primary_slot();
			cursor_display.get_component<ui::UiBounds>().local.center = world.get_resource<userinput::InputManager>().mouse_position;
		for (auto&& [interaction_state, container_slot,decl] : ecs::View< ui::InteractionState, items::RefrencedSlot,   items::ItemSlotDecal>(world)) {
				if (!interaction_state.right_clicked) {
					continue;
				}
				if (cursor_slot.get<ElementSlot>().empty() && container_slot.displayed_object().is_none()) {
					continue;
				}
				//implies container_slot is nonempty;
				if (cursor_slot.get<ElementSlot>().empty()) {

					items::apply_plan(TransferSlotsRequest{ .from_slot = container_slot.displayed(),.to_slot = cursor_slot, .count = container_slot.entry().unwrap().count / 2 });
					
				}
				else {
					items::apply_plan(TransferSlotsRequest{ .from_slot = cursor_slot,.to_slot = container_slot.displayed(), .count = 1 });
				}
				return;

			}
		}
	};
	struct cursor_highlighter :ecs::System {
		void run(ecs::Ecs& world) {
			cursor_container& cont = world.get_resource<cursor_container>();
			cont.display.get_component<ui::UiEnabled>().enable();
			ecs::Constrained<ElementSlot> cursor_obj = cont.primary_slot();
			ui::ComputedStyle & style = ecs::parent(cont.display)
			.unwrap()
			.get_component<ui::ComputedStyle>();

			ElementSlot& cursor_slot = cursor_obj.get_component<ElementSlot>();
			for (auto [item_decal, interaction_state] : ecs::View< ItemSlotDecal, ui::InteractionState>(world)) {
				if (interaction_state.hovered&&world.get_resource<ui::MenuState>().menu_open()) {
						item_decal.set_decal(renderer::TexturePath("images\\importantblockholder.png"));
				}
				else {
					item_decal.reset_decal();
				}
			}
		}
	};



	inline void cursor_container_plugin(core::App& app) {
			ecs::obj cursor_entity = ecs::spawn(app.Ecs, items::ItemSlotSpawner());
			ecs::obj cursor_display = ecs::spawn(app.Ecs, items::ClearItemSlotSpawner(ecs::Constrained<items::ElementSlot>(cursor_entity)));
				
			app.emplace_system<cursor_swapper>();
			app.emplace_system<cursor_spreader>();
			app.emplace_system<cursor_highlighter>();
			app.emplace_resource<cursor_container>(cursor_entity, cursor_display);
	};

};