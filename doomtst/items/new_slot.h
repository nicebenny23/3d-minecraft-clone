#include "new_item.h"
#include "item_ui.h"
#include "../renderer/uibox.h"

namespace slot {
	constexpr v2::Coord2 grid_size = v2::Coord2(1, 3);

	struct ItemSlot:gameobject::component {
	
		bool has() const {

			return static_cast<bool>(held);
		}
		bool empty() const {

			return !static_cast<bool>(held);
		}
		void swap(ItemSlot& oth) {

			std::swap(oth.held, held);
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
		private:
			Opt::Option<gameobject::obj> held;
		

	};
	struct UiSlot :gameobject::component {


		
		
		void setdecal(const char* location, const char* name) {
			if (!framedecal.valid())
			{
				framedecal = ui::createuielement<uiboxname::uibox>("images\\blockholder.png", "DefaultItemSlotTexture", grid_size,grid_size, 13);
			}
			framedecal->LoadTex(location, name);

		}
		void disable() {
			if (owner.hascomponent<)
			{

			}

		}






		
	

		private:
		v2::Coord2 location;
		Cptr::cptr<uiboxname::uibox> framedecal;

	};
}
