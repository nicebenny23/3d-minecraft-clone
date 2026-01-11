#include "block.h"
#include "../items/menu.h"
#include "../math/vector2.h"
#include "../world/voxeltraversal.h"
#include "../game/objecthelper.h"
#include "../items/recipe.h"
#pragma once 
struct furnacemenu :menu {

	recipemanager blkcont;
	ui_image progressbar;
	furnacemenu(v2::Vec2 size):menu(size), progressbar(*CtxName::ctx.Ecs, "images\\greenbar.png", "GreenBarTexture", geo::Box2d(v2::Vec2(0, .1), v2::zerov), 100){
		menubox =new ui_image(*CtxName::ctx.Ecs,"images\\menutex.png", "MenuTexture", geo::Box2d::origin_centered(size), 1);
	
		menubox->disable();
		menutype = normalmenu;
		blkcont = recipemanager("crafting\\furnace.txt", 1,1);
		progressbar.disable();
		enabled = false;
		blkcont.attributes = extrarecipeattributes(true, 2);
	}
	void custominit() {

	}
	void save() {



	}
	void customopen() {
		progressbar.enable();
		enabled = true;
		blkcont.enable();
	}
	void customclose() {
		progressbar.disable();
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
		men.progressbar.set_bounds(progbox);

	
		
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
	furnacecomp():men(v2::Vec2(3, 3)) {

	}
	void destroy_hook() {
	
			men.blkcont.save();
		
		men.blkcont.destroy();
	}
	void start() {

		men = std::move(furnacemenu(v2::Vec2(3, 3)));
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
	blk.owner().ensure_component<furnacecomp>();
	blk.owner().get_component<furnacecomp>().men.blkcont.attributes.timetocraft = 1;
	blk.owner().get_component<furnacecomp>().men.blkcont.attributes.isauto = true;
	blk.owner().add_component<loottable>().addelem(furnaceitem, 1, false);
}