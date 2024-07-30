#include "itemslot.h"
#include "../util/mathutil.h"

void destroyonclick(itemslot& toremove) {
	toremove.helditem->itemui.itemsprite->shouldrender = false;
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
	important = false;
	
	important = false;
}


void itemslot::givefreeamt(int amt)
{


	if (amt<=0)
	{
		return;
	}
	if (helditem==nullptr)
	{
		Assert("cant give item to other from nullptr");
	}
	if (freeditem==nullptr)
	{
		freeditem = inititem(helditem->id,0);
		
		freeditem->setviewable(frame->shouldrender);
		
		freeditem->state = beingheld;
	}
	freeditem->amt += amt;
	helditem->amt -= amt;
	freeditem->updateui();
	freeditem->itemui.itemsprite->box.center = frame->box.center;


}
void itemslot::giveitem(int id,int amt) {
	if (id == 0)
	{
		helditem = nullptr;
		return;
	}
	helditem = inititem(id,amt);
	helditem->setviewable(frame->shouldrender);
	helditem->state = ininventoryblock;
	
	helditem->itemui.itemsprite->box.center = frame->box.center;

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

		helditem->itemui.itemsprite->box.center = frame->box.center;

	}

}

void itemslot::destroyitem() {
	if (helditem != nullptr) {
		helditem->destroy();
		helditem = nullptr;
	}
}

void itemslot::enable() {
	frame->shouldrender = true;
	if (helditem != nullptr && helditem->itemui.itemsprite != nullptr) {
		helditem->setviewable(true);
	}
}
void itemslot::makeimportant()
{
	if (!important) {
		frame->tex.destroy();
		frame->tex = texture("images\\importantblockholder.png", png);
		important = true;
	}
}
void itemslot::makeunimportant()
{
	if (important)
	{
		frame->tex.destroy();
		frame->tex = texture("images\\blockholder.png", png);
		important = false;
	}
	
	
}
void itemslot::disable() {
	frame->shouldrender = false;
	if (helditem != nullptr && helditem->itemui.itemsprite != nullptr) {
		helditem->setviewable(false);
	}
}
bool itemslot::hasbeenleftclicked()
{
	
		if (frame->mouseonblock() && userinput::mouseleft.pressed) {

			return true;
		}
	
	return false;
}
bool itemslot::hasbeenrightclicked()
{

	if (frame->mouseonblock() && userinput::mouseright.pressed) {

		return true;
	}

	return false;
}

void itemslot::updatestate() {



		
		if (ismenuopen())
		{


			if (hasbeenleftclicked()) {
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

			if (hasbeenrightclicked()) {
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

