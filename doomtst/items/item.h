#include "../renderer/uirender.h"
#include "../renderer/texture.h"
#include "../block/block.h"
#include "../renderer/textrender.h"
#ifndef item_HPP
#define item_HPP
enum itemid {

	stoneitem = 0,
	sworditem = 1,
	grassitem = 2,
	torchitem = 3,
	pickitemid = 4,
};
enum itemstate {
	beingheld = 0,
	ininventoryblock = 1,

};

struct item
{
	void setviewable(bool isviewable) {


		itemui.textvalue->shouldrender = isviewable;
		itemui.itemsprite->shouldrender = isviewable;
	}
	item(int itemid);
	int state;
	item() {
		maxamt = 0;
		amt = 0;
		itemui.textvalue = createinteger(v2::zerov, 1 / 70.f);
	}
	int id;
	float pickaxepower;
	void(*onleft);
	void(*onright);
	bool placeable;
	int maxamt;

	int amt;

	struct  itemuistruct
	{
		integertext* textvalue;
		uirender::uibox* itemsprite;
	};
	itemuistruct itemui;
	void updateui() {
		if (state==beingheld)
		{
			itemui.itemsprite->box.center = userinput::normedmousepos;
		}
		itemui.textvalue->center = itemui.itemsprite->box.center-itemui.itemsprite->box.scale/1.1;
		itemui.textvalue->value= amt;
		
	}
	void destroy();
	bool use(int useamt);
	bool canuse(int useamt);
	
	void maxoutthis(item* itm);
};
extern texturearray textureidlist;
extern item* freeditem;
void createfreeditem();
#endif // !item_HPP
