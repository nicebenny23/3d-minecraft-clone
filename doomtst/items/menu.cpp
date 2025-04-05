#include "menu.h"
#include "../renderer/Window.h"
#include "../util/userinput.h"
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

	if (userinput::getinputkey(esckey).pressed) {
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
			glfwSetWindowShouldClose(window::awindow, true);
		}
	}
}
void menu::close()
{
	if (menutype==inventorymenu)
	{

		enabled = false;
		menubox->state.enabled = false;
		customclose();
		return;
	}

	enabled = false;
	openmenu = nullptr;
	menubox->state.enabled=false;
	customclose();
	inventorylocation->close();
}

void menu::open()
{
	if (menutype==inventorymenu)
	{


		menubox->state.enabled = true;
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
	menubox->state.enabled = true;
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

menu::menu(v2::Vector2 size)
{

	menubox = ui::createuielement<uibox>("images\\menutex.png", size,v2::zerov,11);
	menubox->state.enabled = false;
	enabled = false;
}
 void menu::testclick()
{

	
}

 void menu::custominit()
 {

 }
