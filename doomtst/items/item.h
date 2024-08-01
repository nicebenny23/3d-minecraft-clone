#include "../renderer/uirender.h"
#include "../renderer/texture.h"
#include "../block/block.h"
#include "../renderer/textrender.h"
#ifndef item_HPP
#define item_HPP
inline uirender::uibox* createitembox(const char* boxname) {

	return uirender::newbox(boxname, v2::unitv / 40.f, v2::zerov, 100);
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


		itemui.textvalue->shouldrender = isviewable;
		itemui.itemsprite->shouldrender = isviewable;
	}
	item(int itemid);
	
	item() {
		maxamt = 0;
		amt = 0;
		properties.foodval = 0;
		itemui.textvalue = createinteger(v2::zerov, 1 / 70.f);
	}

	struct itemproperties {
		float pickaxepower;
		float foodval;

		int dmg;
		float armor;
		bool placeable;
		itemproperties() {

			pickaxepower = 0;
			foodval = 0;
			dmg = 1;
			armor = 1;
			placeable = false;
		}
		
	};

	struct  itemuistruct
	{
		integertext* textvalue;
		uirender::uibox* itemsprite;
	};

	itemproperties properties;

	
	itemprop itemtype;

	itemuistruct itemui;

	void updateui() {
		if (state==beingheld)
		{
			itemui.itemsprite->box.center = userinput::normedmousepos;
		}
		itemui.textvalue->center = itemui.itemsprite->box.center-itemui.itemsprite->box.scale*v2::Vector2(-1.4,.8)/1.14f;
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
extern texturearray textureidlist;
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
