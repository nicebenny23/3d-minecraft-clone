#include "../renderer/uibox.h"
#include "../renderer/texture.h"
#include "../block/block.h"
#include "../renderer/textrender.h"
#include "../renderer/uibox.h"
#include "../game/Core.h"
#include "../game/GameContext.h"
#ifndef item_HPP
#define item_HPP
inline cptr<uiboxname::uibox> createitembox(const char* boxname,const char* TextureName) {

	return ui::createuielement<uiboxname::uibox>(boxname, TextureName , v2::unitv / 40.f, v2::zerov, 100);
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
//
struct item
{
	int state;
	int id;
	int maxamt;
	int amt;
	void setviewable(bool isviewable) {


		itemui.textvalue->state.enabled = isviewable;
		itemui.itemsprite->state.enabled = isviewable;
	}
	item(int itemid);
	item(int itemid,int amt);
	item() {
		maxamt = 0;
		amt = 0;
		properties.foodval = 0;
		itemui.textvalue = ui::createuielement<integertext>(v2::zerov, 1 / 70.f);
	}

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
		cptr<integertext> textvalue;
		cptr<uiboxname::uibox> itemsprite;
	};

	itemproperties properties;

	
	itemprop itemtype;

	itemuistruct itemui;

	void updateui() {
		if (state==beingheld)
		{
			//adjust for apwect ratio 
			itemui.itemsprite->box.center = CtxName::ctx.Window->FitToAspectRatio( CtxName::ctx.Inp->mousepos);
		}
		itemui.textvalue->center = itemui.itemsprite->box.center-itemui.itemsprite->box.scale;
		itemui.textvalue->value= amt;
		if (itemtype==wear)
		{
			int textamt = round((amt *5)/ maxamt);
			itemui.textvalue->value = textamt;
		}
		
	}
	bool canadd(int amtto);
	void destroy();
	bool use(int useamt);
	bool canuse(int useamt);
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
#endif // !item_HPP
