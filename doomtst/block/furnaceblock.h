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

	furnacemenu(v2::Vector2 size) {
		menubox = newbox("images\\menutex.png", size, v2::zerov, 1);

		menubox->shouldrender = false;
		menutype = normalmenu;

		blkcont = recipemanager("furnace.txt", 1,1);
		
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

	furnacemenu() = default;



};
struct furnacecomp :gameobject::component {

	furnacemenu men;
	void onplayerclick() {
		if (userinput::getinputkey('p').pressed)
		{
			men.open();
		}

		int l = 1;
	}
	void update() {
		blockname::block* getblockbelow = grid::getobjatgrid(objutil::toblk(owner).pos - Coord(0, 1, 0));
		men.blkcont.state.cancraft = false;
		if (getblockbelow != nullptr)
		{

			if (getblockbelow->id==minecraftlava)
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
	blk->solid = true;
	blk->transparent = false;
	blk->emitedlight = 0;
	blk->mininglevel = 1;
	blk->mesh.scale = blockname::unitscale;

	blk->createaabb(false);
	//stupid thing
	if (!blk->hascomponent<furnacecomp>())
	{

		blk->addcomponent<furnacecomp>();

	}
	blk->addcomponentptr<loottable>()->addelem(furnaceitem, 1, false);
}
inline void furnacedelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

	blk->removecomponent<furnacecomp>();

	blk->removecomponent<loottable>();
}
#endif // !wood_HPP
