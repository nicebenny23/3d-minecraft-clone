#include "../renderer/uibox.h"


using namespace ui;
using namespace stn;
#pragma once 
enum type {
	inventorymenu=1,
	normalmenu=2,
	settingsmenu=3,
};
bool ismenuopen();
struct menu
{
	
	void close();
	void(*closeinven);
	ui_image* menubox;
		type menutype;
		
	bool enabled;
	void open();
	virtual void customclose();
	//just for now while we have to deal with this horrific abstraction
	void prophecy_of_ra() {
		menubox = new ui_image(*CtxName::ctx.Ecs, "images\\menutex.png", "MenuTexture", geo::Box2d::origin_centered(v2::unitv / 2), 11);
	}
	virtual void customopen();
	menu(v2::Vec2 size);
	virtual void testclick();
	virtual void custominit();
};
;
extern menu* openmenu;
extern menu* inventorylocation;
void managemenus();
 // ! menu_HPP
