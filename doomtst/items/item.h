#include "../renderer/uirender.h"
#ifndef item_HPP
#define item_HPP
enum itemid {
	stoneitem=1,
	sworditem=2,

};
enum itemstate {
	beingheld=0,
	ininventoryblock=1,
	inmainhand=2,

};
struct item
{

	item(int itemid);
	int state;
	item() {
	}
	int id;
	void(*onleft);
	void(*onright);
	bool placeable;
	int maxamt;
	uirender::uibox* itemsprite;
	
};
#endif // !item_HPP
