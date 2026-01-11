#include "item.h"
#pragma once 
#define airitemasblockid 0
inline void initairitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->itemui.itemsprite = createitembox("images\\airtex.png", "AirTexture");
}

 // !airinit_HPP
