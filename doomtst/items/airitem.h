#include "item.h"
#pragma once 
#define airitemasblockid 0
inline void initairitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->itemui.itemsprite = ui::createuielement<uiboxname::uibox>("images\\airtex.png", "AirTexture", v2::Vector2(1 / 20.f, 1 / 20.f), v2::zerov, 100);
}

 // !airinit_HPP
