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
	inventory();
		Container hotbar;
		item* selected;
		
	void update() {
		
		hotbar.update();
		if (userinput::getinputkey('j').pressed) {
			playermenu.close();
		}

		if (userinput::getinputkey('e').pressed) {
			playermenu.open();
		}
		if (userinput::getinputkey('1').pressed)
		{
			selected = hotbar.databuf[0].helditem;
		}
		if (userinput::getinputkey('2').pressed)
		{
			selected = hotbar.databuf[1].helditem;
		}
		if (userinput::getinputkey('3').pressed)
		{
			selected = hotbar.databuf[2].helditem;
		}if (userinput::getinputkey('4').pressed)
		{
			selected = hotbar.databuf[3].helditem;
		}
		if (userinput::getinputkey('5').pressed)
		{
			selected = hotbar.databuf[4].helditem;
		}
		if (userinput::getinputkey('6').pressed)
		{
			selected = hotbar.databuf[5].helditem;

		}
		if (freeditem!=nullptr)
		{
			
			freeditem->updateui();

			if (freeditem->amt == 0) {
				freeditem->destroy();
				freeditem = nullptr;
			}

		}
		if (openmenu!=nullptr)
		{
			playermenu.manager.disable();
		}
		
		playermenu.testclick();

		
	}
};



#endif // !playerinventory_H
