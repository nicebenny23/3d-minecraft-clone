#pragma once
#include "menu.h"
#include "../util/userinput.h"
#include "../game/Core.h"
#include "../game/GameContext.h"
menu* openmenu;
menu* inventorylocation;
bool ismenuopen() {
	
	if (openmenu!=nullptr)
	{
		return true;
	}
	if (inventorylocation->enabled)
	{
		return true;
	}
	return false;
}
void managemenus()
{

	if (CtxName::ctx.Inp->getKey(esckey).pressed) {
		if (openmenu != nullptr)
		{
			openmenu->close();
			return;
		}
		if (inventorylocation&&inventorylocation->enabled)
		{
			inventorylocation->close();
			return;
		}
		else {
			glfwSetWindowShouldClose(CtxName::ctx.Window->WinPtr, true);
		}
	}
}
void menu::close()
{
	if (menutype==inventorymenu)
	{

		enabled = false;
		menubox->disable();
		customclose();
		return;
	}

	enabled = false;
	openmenu = nullptr;
	menubox->enabled() = false;
	customclose();
	inventorylocation->close();
}

void menu::open()
{
	if (menutype==inventorymenu)
	{


		menubox->enable();
		enabled = true;
		customopen();
		return;
	}
	
	
	if (enabled)
	{
		return;
	}


	
	if (openmenu!=nullptr)
	{
			openmenu->close();
	}
	menubox->enable();
	enabled = true;
	openmenu = this;
	customopen();
	if (menutype == settingsmenu)
	{
		return;
	}
	
	inventorylocation->open();
	
}

void menu::customclose()
{
	
}

void menu::customopen()
{

}

menu::menu(v2::Vec2 size)
{

	menubox = ui::createuielement<ui_image_component>("images\\menutex.png", "MenuTexture", size,v2::zerov,11);
	menubox->disable();
	enabled = false;
}
 void menu::testclick()
{

	
}

 void menu::custominit()
 {

 }
