#pragma once
#include "../debugger/debug.h"
#include "itemslot.h"
#include "../util/mathutil.h"

void destroyonclick(itemslot& toremove) {
	toremove.helditem->itemui.itemsprite->state.enabled= false;
}
Box2d getboxfrominvloc(int xloc, int yloc) {

	float xval = ((2 * xloc + 1.0f) / float(xamt));
	float yval = ((2 * yloc + 1.0f) / float(yamt));
	v2::Vec2 scale = v2::Vec2(1.0f / float(xamt), 1.0f / float(yamt));
	return Box2d(v2::Vec2(xval, yval), scale);
}

itemslot::itemslot(int xloc, int yloc) {
	Box2d frameboxsize = getboxfrominvloc(xloc, yloc);
	framedecal = ui::createuielement<uibox>("images\\blockholder.png", "DefaultItemSlotTexture",frameboxsize.scale, frameboxsize.center, 13);
	helditem = nullptr;

	dtype = normaldecal;
	
}


bool itemslot::givefreeamt(size_t amt)
{

	
	
	if (helditem==nullptr)
	{
		throw("cant give item to other from nullptr");

		return false;
	}
	if (freeditem==nullptr)
	{
		freeditem = inititem(helditem->id,0);
		
		freeditem->setviewable(framedecal->state.enabled);

		freeditem->state = beingheld;
	}
	if (freeditem->canadd(amt))
	{


		freeditem->amt += amt;
		helditem->amt -= amt;
		freeditem->updateui();
		freeditem->itemui.itemsprite->box.center = framedecal->box.center;
		return true;
	}
	return false;

}
void itemslot::giveitem(int id,int amt) {
	if (id == 0)
	{
		helditem = nullptr;
		return;
	}
	helditem = inititem(id,amt);
	helditem->setviewable(framedecal->state.enabled);
	helditem->state = ininventoryblock;
	
	helditem->itemui.itemsprite->box.center = framedecal->box.center;

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

		helditem->itemui.itemsprite->box.center = framedecal->box.center;

	}

}

void itemslot::transfer(itemslot& other)
{
	
	if (other.has())
	{
		throw std::logic_error("Unable to give item to a full item slot");
	}

}

void itemslot::destroyitem() {
	if (helditem != nullptr) {
		helditem->destroy();
		helditem = nullptr;
	}
}

void itemslot::enable() {
	
	framedecal->state.enabled = true;
	if (helditem != nullptr && helditem->itemui.itemsprite.ptr() != nullptr) {
		helditem->setviewable(true);
	}
}
void itemslot::setdecal(decaltype toset)
{
	if (toset==dtype)
	{
		return;
	}
	if (dtype==destroydecal)
	{
		return;
	}
	dtype = toset;
	if (toset==importantdecal) {
		
		framedecal->LoadTex("images\\importantblockholder.png","ImportantBlockHolderTexture");
	
		return;
	}
	if (toset == normaldecal) {
		framedecal->LoadTex("images\\blockholder.png","BlockHolderTexture");
	
		return;
	}
	if (toset == destroydecal) {

		framedecal->LoadTex("images\\x.png","XTexture");
		
		return;
	}
	if (toset == leggingdecal) {

		framedecal->LoadTex("images\\leggingdecal.png","LeggingDecalTexture");

		return;
	}
	if (toset == chestdecal) {

		framedecal->LoadTex("images\\chestdecal.png", "ChestDecalTexture");

		return;
	}
}
void itemslot::disable() {

	framedecal->state.enabled = false;
	if (helditem != nullptr && helditem->itemui.itemsprite.ptr() != nullptr) {
		helditem->setviewable(false);
	}
}


void itemslot::updatestate() {



		
		if (ismenuopen())
		{


			if (framedecal->state.leftclicked) {
				if (freeditem != nullptr && helditem != nullptr)
				{

					if (helditem->itemtype == count)
					{


						if (freeditem->id == helditem->id) {
							helditem->maxoutthis(freeditem);
							return;
						}
					}
				}

				transferitem(freeditem);
			}

			if (framedecal->state.rightclicked) {
				if (helditem != nullptr)
				{
					int giveamt = helditem->amt / 2;
					if (freeditem == nullptr || freeditem->id == helditem->id)
					{
						if (helditem->itemtype == count)
						{
							givefreeamt(giveamt);
						}
						
					}


				}
			}
		}
		
			updateitem(helditem);
		
		
	
}

