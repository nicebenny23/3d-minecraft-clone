#include "item.h"
#pragma once 
#define airitemasblockid 0
inline void initairitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->itemui.itemsprite = ui::createuielement<uiboxname::ui_image_component>("images\\airtex.png", "AirTexture", v2::Vec2(1 / 20.f, 1 / 20.f), v2::zerov, 100);
}

 // !airinit_HPP
