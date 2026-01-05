#include "../items/menu.h"
#include "../game/ecs/game_object.h"
#include "../util/userinput.h"
#include "../items/recipe.h"
#pragma once 
struct inventorymen :menu
{
	Container blkcont;
	Container armor;
	Container deletecont;
	recipemanager manager;
	inventorymen(v2::Vec2 size) {
		menubox = ui::createuielement<ui_image_component>("images\\menutex.png", "MenuBoxTexture", size, v2::zerov, 11);
		
		menubox->disable();
		
		manager=recipemanager("crafting\\2x2craft.txt", 1, 1);
		blkcont = Container(8, 4, 0, 0);
		armor= Container(1, 2, -4, 4);
		deletecont = Container(1, 1, 5, -5);
		enabled = false;
		armor.at(0).setdecal(leggingdecal);
		armor.at(1).setdecal(chestdecal);
	}
	void custominit() {

	}
	void customopen() {
		deletecont.enable();
		manager.enable();
		blkcont.enable();
		armor.enable();
	}
	void customclose() {
		deletecont.disable();
		armor.disable();
		blkcont.disable();
		manager.disable();
	}
	void testclick() {


		if (enabled)
		{
			deletecont.update();
			deletecont.at(0).setdecal(destroydecal);
			deletecont.at(0).destroyitem();
			manager.updatestate();
			blkcont.update();
			armor.update();
		}

	}

	inventorymen() = default;
};
struct inventory : ecs::component
{
	Container blkcont;
	inventorymen playermenu;
	void givestartitems(int i1, int i2,int i3);
	inventory();
	unsigned int selectedind;
		Container hotbar;
		item* selected;
		
	void update() {
		
		hotbar.update();
		if(CtxName::ctx.Inp->getKey('j').pressed) {
			playermenu.close();
		}
		int prevselectedind = selectedind;
		if (CtxName::ctx.Inp->getKey('e').pressed) {
			playermenu.open();
		}
		if (CtxName::ctx.Inp->getKey('1').pressed)
		{
			
			selectedind = 0;
		}
		if (CtxName::ctx.Inp->getKey('2').pressed)
		{
			selectedind = 1;
		}
		if (CtxName::ctx.Inp->getKey('3').pressed)
		{

			selectedind = 2;
		}if (CtxName::ctx.Inp->getKey('4').pressed)
		{
			selectedind = 3;
		}
		if (CtxName::ctx.Inp->getKey('5').pressed)
		{

			selectedind = 4;
		}
		if (CtxName::ctx.Inp->getKey('6').pressed)
		{
			selectedind = 5;

		}
		if (prevselectedind != selectedind)
		{



			if (prevselectedind != -1)
			{
				hotbar.at(prevselectedind).setdecal(normaldecal);
			}
			if (selectedind!=-1)
			{

				hotbar.at(selectedind).setdecal(importantdecal);

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
			playermenu.armor.disable();

			playermenu.manager.disable();
		}
		
		playermenu.testclick();

		
	}
};



 // !playerinventory_H
