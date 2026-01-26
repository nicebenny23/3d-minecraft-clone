#pragma once
#include "playerinventory.h"
void inventory::givestartitems(std::string items) {
	int dropamt = 1;
	items::item_id id = world().get_resource<items::item_type_register>().unwrap().from_name(items);
	items::item_entry entry = items::item_entry(id, 10);
	stn::Option<items::AddToSlotPlan> plan=items::give_container_entry(entry, hotbar.get_component < items::container>());
	if (plan) {
		plan.unwrap().apply(world());
	}
}
