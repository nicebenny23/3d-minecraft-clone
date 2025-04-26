#include "item.h"
#ifndef ironbottomitem_HPP
#define ironbottomitem_HPP
#define ironbottomitemblockid -1
inline void initironbottomitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->properties.placeable = false;
	itm->properties.dmg = 0;
	itm->properties.armor = .8;
	itm->itemtype = wear;
	itm->itemui.itemsprite = createitembox("images\\ironbottom.png","IronBottomTexture");
}
#endif // !ironsworditem_HPP
