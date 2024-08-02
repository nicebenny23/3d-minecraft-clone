#include "menu.h"
#include "../renderer/Window.h"
#include "../util/userinput.h"
menu* openmenu;
menu* inventorylocation;
bool ismenuopen() {
	if (openmenu!=nullptr)
	{
		return true;
	}if (inventorylocation->enabled)
	{
		return true;
	}
	return false;
}
void managemenus()
{

	if (userinput::esckey.pressed) {
		if (openmenu != nullptr)
		{
			openmenu->close();
			return;
		}
		if (inventorylocation->enabled)
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
		menubox->shouldrender = false;
		customclose();
		return;
	}

	enabled = false;
	openmenu = nullptr;
	menubox->shouldrender=false;
	customclose();
	inventorylocation->close();
}

void menu::open()
{
	if (menutype==inventorymenu)
	{


		menubox->shouldrender = true;
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
	
	inventorylocation->open();
			menubox->shouldrender = true;
			enabled = true;
			openmenu = this;
			customopen();
}

void menu::customclose()
{
	
}

void menu::customopen()
{

}

menu::menu(v2::Vector2 size)
{

	menubox = newbox("images\\menutex.png", size,v2::zerov,11);
	menubox->shouldrender = false;
	enabled = false;
}
 void menu::testclick()
{

	
}

 void menu::custominit()
 {
	 int l = 1;

 }
