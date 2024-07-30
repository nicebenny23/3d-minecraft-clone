#include "../items/menu.h"
#include "../game/gameobject.h"
#include "../util/userinput.h"
#include "../items/recipe.h"
#ifndef playerinventory_Hpp
#define playerinventory_Hpp
struct inventorymen :menu
{
	Container blkcont;
	recipemanager manager;
	inventorymen(v2::Vector2 size) {
		menubox = newbox("images\\menutex.png", size, v2::zerov, 11);
		
		menubox->shouldrender = false;
		
		manager=recipemanager("2x2craft.txt", 1, 1);
		blkcont = Container(8, 4, 0, 0);
		enabled = false;
		 
	}
	void custominit() {

	}
	void customopen() {

		manager.enable();
		blkcont.enable();
	}
	void customclose() {

		blkcont.disable();
		manager.disable();
	}
	void testclick() {


		if (enabled)
		{
			manager.updatestate();
			blkcont.update();
		}

	}

	inventorymen() = default;
};
struct inventory :gameobject::component
{
	Container blkcont;
	inventorymen playermenu;
	void givestartitems(int i1, int i2);
	inventory();
	unsigned int selectedind;
		Container hotbar;
		item* selected;
		
	void update() {
		
		hotbar.update();
		if(userinput::getinputkey('j').pressed) {
			playermenu.close();
		}
		int prevselectedind = selectedind;
		if (userinput::getinputkey('e').pressed) {
			playermenu.open();
		}
		if (userinput::getinputkey('1').pressed)
		{
			
			selectedind = 0;
		}
		if (userinput::getinputkey('2').pressed)
		{
			selectedind = 1;
		}
		if (userinput::getinputkey('3').pressed)
		{

			selectedind = 2;
		}if (userinput::getinputkey('4').pressed)
		{
			selectedind = 3;
		}
		if (userinput::getinputkey('5').pressed)
		{

			selectedind = 4;
		}
		if (userinput::getinputkey('6').pressed)
		{
			selectedind = 5;

		}
		if (prevselectedind != selectedind)
		{



			if (prevselectedind != -1)
			{
				hotbar.at(prevselectedind).makeunimportant();
			}
			if (selectedind!=-1)
			{

				hotbar.at(selectedind).makeimportant();

			}
		}
		if (selectedind!=-1&&hotbar.at(selectedind).helditem!=nullptr)
		{
		
			
			selected = hotbar.at(selectedind).helditem;

		
		}
		else
		{
			
			selected = nullptr;
		}
		prevselectedind = selectedind;
		updateitem(freeditem);
		if (openmenu!=nullptr)
		{
			playermenu.manager.disable();
		}
		
		playermenu.testclick();

		
	}
};



#endif // !playerinventory_H
