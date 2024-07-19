#include "inventoryblock.h"
#include "../util/mathutil.h"

void destroyonclick(itemslot& toremove) {
	toremove.helditem->itemsprite->shouldrender = false;
}
Box2d getboxfrominvloc(int xloc, int yloc) {

	float xval = ((2 * xloc + 1.0f) / float(xamt));
	float yval = ((2 * yloc + 1.0f) / float(yamt));
	v2::Vector2 scale = v2::Vector2(1.0f / float(xamt), 1.0f / float(yamt));
	return Box2d(v2::Vector2(xval, yval), scale);
}

itemslot::itemslot(int xloc, int yloc) {
	Box2d frameboxsize = getboxfrominvloc(xloc, yloc);
	frame = uirender::newbox("images\\blockholder.png", frameboxsize.scale, frameboxsize.center, 13);
	helditem = nullptr;
	if (randombool())
	{
		giveitem(1);
	}
	onclick = destroyonclick;

}

itemslot::itemslot(int xloc, int yloc, void(*clickaction)(itemslot&)) {
	Box2d frameboxsize = getboxfrominvloc(xloc, yloc);
	frame = uirender::newbox("images\\blockholder.png", frameboxsize.scale, frameboxsize.center, -130);


	onclick = clickaction;
}

void itemslot::giveitem(int id) {
	helditem = inititem(2);
	helditem->amt = 1;
	helditem->state = ininventoryblock;
	helditem->itemsprite->box.center = frame->box.center;
	helditem->itemsprite->box.scale = frame->box.scale / 1.4f;
}

void itemslot::transferitem(item* otherholder) {


	item* temp = otherholder;
	freeditem = helditem;
	helditem = otherholder;
	if (freeditem != nullptr)
	{

		freeditem->state = beingheld;

	}
	if (helditem != nullptr)
	{

		helditem->state = ininventoryblock;

		helditem->itemsprite->box.center = frame->box.center;

	}

}

void itemslot::destroyitem() {
	if (helditem != nullptr) {
		helditem->destroy();
		helditem = nullptr;
	}
}

void itemslot::setviewable(bool isviewable) {
	frame->shouldrender = isviewable;
	if (helditem != nullptr && helditem->itemsprite != nullptr) {
		helditem->itemsprite->shouldrender = isviewable;
	}
}

void itemslot::testclick() {
	if (frame->mouseonblock() && userinput::mouseleft.pressed) {
		transferitem(freeditem);
	}
}

