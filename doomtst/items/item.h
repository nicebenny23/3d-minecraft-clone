
#include "../block/block.h"
#include "../renderer/textrender.h"
#include "../game/Core.h"
#include "../game/GameContext.h"
#include "../renderer/uibox.h"

#pragma once 
inline ui::ui_image* createitembox(const char* boxname,const char* TextureName) {

	return new ui::ui_image(*CtxName::ctx.Ecs, boxname, TextureName , geo::Box2d::origin_centered(v2::unitv / 40.f), 100.f);
}
enum itemid {

	nullitemid = 0,
	stoneitem = 1,
	plankitem = 2,
	crystalitem =3,
	metalroditem=4,
	torchitem = 5,
	sworditem =6,
	pickitemid = 7,
	craftingtableitem=8,
	crystalpickitemid=9,
	simplerock=10,
	slimeballitem= 11,
	crystaltorchitem=12,
	mossitem=13,
	ropeitem=14,
	chestitem=15,
	furnaceitem=16,
	ironoreitem=17,
	ironitem=18,
	ironpickitem=19,
	ironsworditem=20,
	crystaldaggeritem=21,
	irontop=22,
	ironbottom=23,
	crystaltop=24,
	crystalbottom=25,
	treestoneitem=26,
	altaritem=27,
	obsidianitem=28,
	sanditem=29,
	glassitem=30,
	glassbottleitem=31,
	lavabottleitem=32,
	playertpsword=33,
	slimewanditem=34,
};
enum itemstate {
	beingheld = 0,
	ininventoryblock = 1,

};
enum itemprop
{
	wear=0,
	count=1
};

struct item
{
	int state;
	int id;
	int maxamt;
	int amt;
	void setviewable(bool isviewable) {

		if (isviewable) {
			itemui.textvalue.enable();
		}
		else {
			itemui.textvalue.disable();
		}
	}
	item(int itemid);
	item(int itemid,int amt);
	
	


	struct itemproperties {
		float pickaxepower;
		float foodval;
		int onuse;
		int dmg;
		float armor;
		bool placeable;
		itemproperties() {
			onuse = -1;
			pickaxepower = 0;
			foodval = 0;
			dmg = 1;
			armor = 1;
			placeable = false;
		}
		
	};

	struct  itemuistruct
	{
		ui::ui_text textvalue;
		ui::ui_image* itemsprite;
		itemuistruct():itemsprite(nullptr),
		textvalue(*CtxName::ctx.Ecs,geo::Box2d::origin_centered(v2::unitv*1 / 80.0f),67){

		}
	};

	itemproperties properties;

	
	itemprop itemtype;

	itemuistruct itemui;

	void updateui() {
		if (state==beingheld)
		{
			//adjust for apwect ratio 
			itemui.itemsprite->set_center(CtxName::ctx.Window->FitToAspectRatio(CtxName::ctx.Inp->mousepos));
		}
		double text_offset_ideal = .01f;
		itemui.textvalue.set_center(itemui.itemsprite->bounds().center -v2::unitv*text_offset_ideal);
		itemui.textvalue.format("{}", amt);
		if (itemtype==wear)
		{
			int textamt = round((amt *5)/ maxamt);
			itemui.textvalue.format("{}", textamt);
		}
		
	}
	bool canadd(int amtto);
	void destroy();
	bool use(size_t useamt);
	bool canuse(size_t useamt);
	void give(int& givenamt);
	void maxoutthis(item* itm);
};
extern item* freeditem;

inline void updateitem(item*& itm) {
	if (itm == nullptr)
	{
		return;
	}

		itm->updateui();
		if (itm->maxamt < itm->amt)
		{
			itm->amt = itm->maxamt;
		}
		if (itm->amt <= 0) {
			itm->destroy();
			itm = nullptr;
		}
	

}
 // !item_HPP
