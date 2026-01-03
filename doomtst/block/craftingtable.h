#include "block.h"
#include "../items/recipe.h"
#include "../items/menu.h"
#include "../math/vector2.h"
#pragma once 
struct craftingmenu :menu {

	recipemanager blkcont;
	
	craftingmenu(v2::Vec2 size) {
		menubox = ui::createuielement<uibox>("images\\menutex.png", "MenuTexture", size, v2::zerov, 11);

		menubox->disable();
		menutype = normalmenu;
	
		blkcont = recipemanager("crafting\\3x3craft.txt", 3, 3);
		enabled = false;

	}
	void custominit() {

	}
	void save() {

	
		
	}
	void customopen() {

		
		blkcont.enable();
	}
	void customclose() {

		blkcont.disable();
		
	}
	void testclick() {


		if (enabled)
		{
		
			blkcont.updatestate();
		}

	}

	craftingmenu() = default;



};
struct craftingtablecomp : ecs::component{
	
	craftingmenu men;
	void onplayerclick() {
			
			if (CtxName::ctx.Inp->mouseright().pressed)
			{
				men.open();
			}
		
	}
	void update() {

		men.testclick();
		if (CtxName::ctx.Inp->getKey('j').pressed)
		{
			men.close();
		}
	}
	void destroy_hook(){
		
		men.blkcont.save();
		
		men.blkcont.destroy();
	}
	void start() {

		men = craftingmenu(v2::Vec2(3, 3));
	}
};
inline void tableinit(blockname::block& blk) {

	blk.mesh.setfaces(craftingtableside, craftingtableside,craftingtabletop, craftingtableside, craftingtableside, craftingtableside);
	blk.attributes.solid = true;
	blk.attributes.transparent = false;
	blk.emitedlight = 0;
	blk.mininglevel = 1;
	blk.mesh.box.scale = blockname::blockscale;

	blk.createdefaultaabb(false);
	//stupid thing
	if (!blk.owner().has_component<craftingtablecomp>())
	{

		blk.owner().add_component<craftingtablecomp>();

	}
	blk.owner().add_component<loottable>().addelem(craftingtableitem, 1, false);
}