#include "item.h"
#ifndef airitem_HPP
#define airitem_HPP
#define airitemasblockid 0
inline void initairitem(item* itm) {
	itm->pickaxepower = 0;
	itm->itemui.itemsprite = uirender::newbox("images\\airtex.png", v2::Vector2(1 / 20.f, 1 / 20.f), v2::zerov, 100);
}

#endif // !airinit_HPP
