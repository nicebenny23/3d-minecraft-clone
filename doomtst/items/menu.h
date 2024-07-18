#include "../renderer/uirender.h"
#include "inventoryblock.h"
#include "itemstorage.h"
using namespace uirender;
using namespace dynamicarray;
#ifndef  menu_HPP
#define menu_HPP

struct menu
{

	void close();
	void(*closeinven);
		uibox* menubox;
		
		Container blkcont;
	bool isopen;
	void open();
	virtual void customclose();

	virtual void customopen();
	menu() = default;
	menu(v2::Vector2 size);
	void testclick(item* held);
	virtual void onopen();
	virtual void onclose();
};

extern menu* openmenu;

#endif // ! menu_HPP
