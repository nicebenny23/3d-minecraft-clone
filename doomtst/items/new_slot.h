#include "new_item.h"
#include "item_ui.h"
#include "../renderer/uibox.h"
#include <stdexcept>
#include "../game/entity.h"
namespace items{
	constexpr v2::Coord2 grid_size = v2::Coord2(1, 3);
	constexpr v2::Vec2 grid_pixel_scale = v2::Vec2(2 / grid_size.x, 2 / grid_size.y);
	v2::Vec2 slot_center(v2::Coord2 coord) {
		return grid_pixel_scale * v2::Vec2(coord)+grid_pixel_scale/2;
	}
	geometry::Box2d slot_box(v2::Coord2 coord) {
		return geometry::Box2d(slot_center(coord),grid_pixel_scale/2);
	}
	struct ItemSlot:gameobject::component {
	
		bool has() const {

			return static_cast<bool>(held);
		}
		bool empty() const {

			return !static_cast<bool>(held);
		}
		void swap(ItemSlot& oth) {

			oth.held.swap( held);
		}
		void transfer_item(ItemSlot& oth) {
			if (oth.held) {
				throw std::logic_error("Transfer failed: destination ItemSlot must be empty.");
			}
			if (empty()) {
				throw std::logic_error("Transfer failed: source ItemSlot is empty, nothing to transfer.");
			}
			oth.held = std::move(held);
		}
		item_name::item& operator*() {
			return get();
		}
		item_name::item& get() {
			if (empty()) {
				throw std::logic_error("Access error: cannot get item from an empty ItemSlot.");
			}
			return (*held).getcomponent<item_name::item>();
		}	
		size_t fill_amt(item_name::item& itm) {
			if (!held)
			{
				return itm.get_max();
			}
		}
		private:
			gameobject::Observe::entity_slot held;
		

	};
	struct SlotUi :gameobject::component {

		geometry::Box2d slotbox() const {
			return	 slot_box(location);
		}
		
		
		void setdecal(const char* location, const char* name) {
			if (!framedecal.valid())
			{
				framedecal = ui::createuielement<uiboxname::uibox>("images\\blockholder.png", "DefaultItemSlotTexture", slotbox().center, slotbox().scale, 13);
			}
			framedecal->LoadTex(location, name);

		}
		void disable() {
			ItemSlot* slot = owner.getcomponentptr<ItemSlot>();
			if (slot&&slot->has())
			{
				slot->get().owner.getcomponent<item_ui>().disable();
			}
			if (framedecal.valid())
			{
				framedecal->state.enabled = false;	
			}
		}
		item_ui* get() {
			ItemSlot* slot = owner.getcomponentptr<ItemSlot>();
			if (slot && slot->has())
			{
				return slot->get().owner.getcomponentptr<item_ui>();
			}
			return nullptr;
		}
		void enable() {
			ItemSlot* slot = owner.getcomponentptr<ItemSlot>();
			if (slot && slot->has())
			{
				slot->get().owner.getcomponent<item_ui>().enable();
			}
			if (framedecal.valid())
			{
				framedecal->state.enabled = true;
			}
		}
		void bind_box() {
			if (get())
			{
				get()->set_position(slot_center(location));
			}
		}

		private:
		v2::Coord2 location;
		Cptr::cptr<uiboxname::uibox> framedecal;

	};

	struct UiSlot :System {
		void run(gameobject::Ecs* ecs) {

			query::View<SlotUi> items(ecs);
			for (auto [itm] : items) {
				itm->bind_box();
			}
		}


	};
