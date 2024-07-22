#include "../renderer/uirender.h"
#include "../game/gameobject.h"
#include "../game/collision.h"
#include "../world/managegrid.h"
#include "../game/objecthelper.h"
#include "../world/voxeltraversal.h"
#include "../util/dynamicarray.h"
#include "../items/itemstorage.h"
#include "playerinventory.h"
#include "../items/itemutil.h"
using namespace objutil;
#ifndef  playerplace_Hpp
#define playerplace_Hpp
struct playermod:gameobject::component
{
	int curplaceid;
	voxtra::RayCollisionWithGrid closest;
	item* select;
	void start() {
		select = nullptr;
		curplaceid = 0;
		closest.box=nullptr;
	}
	bool caninteract() {

	


		if (closest.box == nullptr)
		{
			return false;
		}
		if (closest.box->owner->type!=gameobject::block)
		{
			return false;
		}
		if (!inrange(closest.dist, interactminrange, interactmaxrange))
		{
			return false;
		}
		return true;
	}
	void placeblock() {
		ray cameraray = ray(Vector3(camera::campos), Vector3(camera::campos) + camera::direction() * 7);
		block* plamentblock = voxtra::findprevblock(cameraray, 1000);
		if (plamentblock->solid)
		{
			return;
		}


		int dir = maxdirection(closest.box->center - plamentblock->center());
		int previd = plamentblock->id;
		//i dont know why i create it and remove itit like this but it makes the core much simpler
		int blockdirection = max2ddirection(Vector3(camera::campos) - closest.colpoint);

		plamentblock->mesh.direction = blockdirection;
		plamentblock->mesh.attachdir = dir;
		gridutil::setblock(plamentblock->pos, blockidfromitemid(select));

		if (plamentblock->solid)
		{


			if (collision::aabbCollidesWithEntity(&plamentblock->getcomponent<Collider>()))
			{
			
				gridutil::setblock(plamentblock->pos, previd);
			}
			else
			{
				select->use(1);
			}
		}




	

	}
	void destroylogic() {

		gridutil::setblock(toblk(closest.box->owner).pos, minecraftair);

	}
	void update() {
		if (ismenuopen())
		{
			return;
		}
		select = owner->getcomponent<inventory>().selected;

		ray cameraray = ray(Vector3(camera::campos), Vector3(camera::campos) + camera::direction() * 7);
		closest = collision::raycastall(cameraray);
		if (!caninteract())
		{
			return;
		}



		if (userinput::mouseleft.pressed)
		{

		
			destroylogic();
		}


		else if (userinput::mouseright.pressed)
		{
			if (select != nullptr)
			{
				if (blockidfromitemid(select) != -1)
				{
					if (select->canuse(1))
					{

						placeblock();
					}
				
				}
			}

		}



	}
};


#endif // ! playerplace_H
