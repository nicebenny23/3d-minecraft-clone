#include "block.h"
#include "../items/menu.h"
#include "../math/vector2.h"
#include "../world/voxeltraversal.h"
#include "../game/objecthelper.h"
#include "../items/recipe.h"
#pragma once 
struct furnacemenu :menu {

	recipemanager blkcont;
	Cptr::cptr<uibox> progressbar;
	furnacemenu(v2::Vec2 size) {
		menubox = ui::createuielement<uibox>("images\\menutex.png", "MenuTexture", size, v2::zerov, 1);
	
		menubox->disable();
		menutype = normalmenu;
		progressbar = ui::createuielement<uibox>("images\\greenbar.png", "GreenBarTexture", v2::Vec2(0, .1) , v2::Vec2(0, 0), 100);
		blkcont = recipemanager("crafting\\furnace.txt", 1,1);
		progressbar->state.enabled= false;
		enabled = false;
		blkcont.attributes = extrarecipeattributes(true, 2);
	}
	void custominit() {

	}
	void save() {



	}
	void customopen() {
		progressbar->state.enabled= true;
		enabled = true;
		blkcont.enable();
	}
	void customclose() {
		progressbar->disable();
		enabled = false;
		blkcont.disable();

	}
	void testclick() {



			blkcont.updatestate();
		

	}

	furnacemenu() = default;



};
struct furnacecomp : ecs::component{

	
	furnacemenu men;

	void onplayerclick() {

		if (CtxName::ctx.Inp->mouseright().pressed) {
			men.open();
		}

		int l = 1;
	}
	void update() {
		float scale1 = (2 - men.blkcont.attributes.timetillcraft) / 10;
		v2::Vec2 center = v2::Vec2(0, .2) + v2::Vec2(scale1/2 , .1);
		v2::Vec2 scale = v2::Vec2(scale1, .03);
		Box2d progbox = Box2d(center, scale/2);
		men.progressbar->box = progbox;

	
		
		blockname::block* getblockbelow = CtxName::ctx.Grid->getBlock(owner().get_component<blockname::block>().pos - Coord(0, 1, 0));
		men.blkcont.state.cancraft = false;
	
		if (getblockbelow != nullptr)
		{

			if (getblockbelow->id == minecraftlava)
			{
				
				men.blkcont.state.cancraft = true;

				if (men.blkcont.state.craftedthisframe)
				{
					getblockbelow->owner().get_component<liquidprop>().liqval -= 1;
				}
			}

		}
		
		men.testclick();
		if (CtxName::ctx.Inp->getKey('j').pressed)
		{
			men.close();
		}
	
	}
	void destroy_hook() {
	
			men.blkcont.save();
		
		men.blkcont.destroy();
	}
	void start() {

		men = furnacemenu(v2::Vec2(3, 3));
	}
};
inline void furnaceinit(blockname::block& blk) {
	
	blk.mesh.setfaces(stonetex, stonetex, furnacefront, stonetex, furnaceside, stonetex);
	blk.attributes.solid = true;
	blk.attributes.transparent = false;
	blk.emitedlight = 0;
	blk.mininglevel = 1;
	blk.mesh.box.scale = blockname::blockscale;

	blk.createdefaultaabb(false);
	//stupid thing
	if (!blk.owner().has_component<furnacecomp>())
	{

		blk.owner().add_component<furnacecomp>();

	}
	

	blk.owner().get_component<furnacecomp>().men.blkcont.attributes.timetocraft = 1;
	blk.owner().get_component<furnacecomp>().men.blkcont.attributes.isauto = true;
	blk.owner().add_component<loottable>().addelem(furnaceitem, 1, false);
}