#include "menu.h"

menu* openmenu;

void menu::close()
{
	isopen = false;
	openmenu = nullptr;
	menubox->shouldrender=false;
	for (int i = 0; i < blockstore.length; i++)
	{
		blockstore[i].hide();
	}
}

void menu::open()
{

	if (isopen)
	{
		return;
	}
	if (openmenu!=nullptr)
	{
			openmenu->close();
	}

			
			menubox->shouldrender = true;
			isopen = true;
			openmenu = this;
			for (int i = 0; i < blockstore.length; i++)
			{
				blockstore[i].show();
			}
}

void menu::customclose()
{
}

void menu::customopen()
{
}

menu::menu(v2::Vector2 size) 
{
	blockstore = array<invblock>();
	int ind = 0;
	for ( int i = -4; i < 4; i++)
	{
		for (int j = -4; j < 4; j++)
		{
			
			blockstore[ind] = invblock(i, j);
			ind++;
		}
	}
	menubox = newbox("menutex.png", size,v2::zerov,-111);
	menubox->shouldrender = true;
}

void menu::testclick(item* held)
{
	for (size_t i = 0; i < blockstore.length; i++)
	{
		
			if (blockstore[i].framebox->mouseonblock())
			{
		
				if (userinput::mouseleft.pressed)
				{
					blockstore[i].transferitem(held);
					//blockstore[i].onclick( blockstore[i]);

				}

			}
		
	}
}
