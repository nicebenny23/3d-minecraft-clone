#include "../renderer/uirender.h"
#include "../renderer/texture.h"
#include "../block/block.h"
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

	item(int itemid);
	int state;
	item() {
		maxamt = 0;
		amt = 0;

	}
	int id;
	float pickaxepower;
	void(*onleft);
	void(*onright);
	bool placeable;
	int maxamt;
	int amt;
	void destroy();
	bool use(int useamt);
	bool canuse(int useamt);
	uirender::uibox* itemsprite;
	void maxoutthis(item* itm);
};
extern texturearray textureidlist;
extern item* freeditem;
void createfreeditem();
#endif // !item_HPP
