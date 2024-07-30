#include "item.h"
#include "../util/vector3.h"
#include "../renderer/uirender.h"
#include  "../util/userinput.h"
#include "itemutil.h"
#include "menu.h"
using namespace v3;
#ifndef invblock_HPP
#define invblock_HPP
const int xamt = 20;
const int yamt = 30;
struct itemslot
{
	itemslot() {
		Assert("invblockconstrutur cant be used just to get complier to work");

	}
	item* helditem;

	//(0...9),(0...14)
	Coord location;
	uirender::uibox* frame;
	
	bool important;
	itemslot(int xloc, int yloc);
	bool empty() {

		return (helditem == nullptr);
	}
	void givefreeamt(int amt);
	void giveitem(int id,int amt);
	void transferitem(item* otherholder);
	void destroyitem();
	void enable();
	void makeimportant();
	void makeunimportant();
	void disable();
	bool hasbeenrightclicked();
	bool hasbeenleftclicked();
	void updatestate();
};

#endif // !invblock_HPP
