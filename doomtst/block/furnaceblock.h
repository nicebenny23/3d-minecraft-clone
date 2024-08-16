#include "block.h"
#include "../items/menu.h"
#include "../util/vector2.h"
#include "../world/voxeltraversal.h"
#include "../game/objecthelper.h"
#include "../items/recipe.h"
#ifndef furnaceblock_HPP
#define furnaceblock_HPP
struct furnacemenu :menu {

	recipemanager blkcont;
	Cptr::cptr<uibox> progressbar;
	furnacemenu(v2::Vector2 size) {
		menubox = ui::createuielement<uibox>("images\\menutex.png", size, v2::zerov, 1);
	
		menubox->state.enabled = false;
		menutype = normalmenu;
		progressbar = ui::createuielement<uibox>("images\\greenbar.png", v2::Vector2(0, .1) , v2::Vector2(0, 0), 100);
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
		progressbar->state.enabled = false;
		enabled = false;
		blkcont.disable();

	}
	void testclick() {



			blkcont.updatestate();
		

	}

	furnacemenu() = default;



};
struct furnacecomp :gameobject::component {

	
	furnacemenu men;

	void onplayerclick() {

		if (userinput::mouseright.pressed) {
			men.open();
		}

		int l = 1;
	}
	void update() {
		float scale1 = (2 - men.blkcont.attributes.timetillcraft) / 10;
		v2::Vector2 center = v2::Vector2(0, .2) + v2::Vector2(scale1/2 , .1);
		v2::Vector2 scale = v2::Vector2(scale1, .03);
		Box2d progbox = Box2d(center, scale/2);
		men.progressbar->box = progbox;

	
		
		blockname::block* getblockbelow = grid::getobjatgrid(objutil::toblk(owner).pos - Coord(0, 1, 0));
		men.blkcont.state.cancraft = false;
	
		if (getblockbelow != nullptr)
		{

			if (getblockbelow->id == minecraftlava)
			{
				
				men.blkcont.state.cancraft = true;

				if (men.blkcont.state.craftedthisframe)
				{
					getblockbelow->getcomponent<liquidprop>().liqval -= 1;
				}
			}

		}
		
		men.testclick();
		if (userinput::getinputkey('j').pressed)
		{
			men.close();
		}
	
	}
	void ondestroy() {
		if (owner->state == gameobject::beingroughdestroyed)
		{
			men.blkcont.save();
		}
		men.blkcont.destroy();
	}
	void start() {

		men = furnacemenu(v2::Vector2(3, 3));
	}
};
inline void furnaceinit(blockname::block* blk) {
	
	blk->mesh.setfaces(stonetex, stonetex, furnacefront, stonetex, furnaceside, stonetex);
	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->mininglevel = 1;
	blk->mesh.scale = blockname::unitscale;

	blk->createaabb(false);
	//stupid thing
	if (!blk->hascomponent<furnacecomp>())
	{

		blk->addcomponent<furnacecomp>();

	}
	

	blk->getcomponent<furnacecomp>().men.blkcont.attributes.timetocraft = 1;
	blk->getcomponent<furnacecomp>().men.blkcont.attributes.isauto = true;
	blk->addcomponent<loottable>()->addelem(furnaceitem, 1, false);
}
inline void furnacedelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

	blk->removecomponent<furnacecomp>();

	blk->removecomponent<loottable>();
}
#endif // !wood_HPP

