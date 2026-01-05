#include "item.h"
#include "../math/vector3.h"
#include "../renderer/uibox.h"
#include  "../util/userinput.h"
#include "itemutil.h"
#include "menu.h"
using namespace v3;
#pragma once 
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
	
	Cptr::cptr<ui_image_component> framedecal;
	decaltype dtype;
	
	itemslot(int xloc, int yloc);
	
	bool givefreeamt(size_t amt);
	void giveitem(int id,int amt);
	void transferitem(item* otherholder);
	bool has() const {
		return helditem != nullptr;

	}
	bool empty() const {
		return helditem == nullptr;

	}
	void transfer(itemslot& other);
	void destroyitem();
	void enable();
	void setdecal(decaltype toset);
	
	void disable();
	void updatestate();
};

 // !invblock_HPP
