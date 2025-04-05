#include "item.h"
#include "../util/vector3.h"
#include "../renderer/uibox.h"
#include  "../util/userinput.h"
#include "itemutil.h"
#include "menu.h"
using namespace v3;
#ifndef invblock_HPP
#define invblock_HPP
const int xamt = 30;
const int yamt = 30;
enum decaltype {

	normaldecal = 0,
	importantdecal = 1,
	destroydecal = 2,
	leggingdecal=3,
	chestdecal=4,
};
struct itemslot
{
	itemslot() {
	//	Assert("invblockconstrutur cant be used just to get complier to work");

	}
	item* helditem;

	//(0...9),(0...14)
	Coord location;
	
	Cptr::cptr<uibox> framedecal;
	decaltype dtype;
	
	itemslot(int xloc, int yloc);
	bool empty() {

		return (helditem == nullptr);
	}
	bool givefreeamt(int amt);
	void giveitem(int id,int amt);
	void transferitem(item* otherholder);
	void destroyitem();
	void enable();
	void setdecal(decaltype toset);
	
	void disable();
	void updatestate();
};

#endif // !invblock_HPP
