#include "../renderer/uirender.h"
#include "../game/gameobject.h"
#include "../game/collision.h"
#include "../world/managegrid.h"
#include "../game/objecthelper.h"
#include "../world/voxeltraversal.h"
#include "../util/dynamicarray.h"
#include "../items/Container.h"
#include "playerinventory.h"
#include "../items/itemutil.h"
using namespace objutil;
#ifndef  playerplace_Hpp
#define playerplace_Hpp
struct playerbreak:gameobject::component
{
	
	voxtra::RayCollisionWithGrid closest;
	item* pickaxe;
	block* currmining;
	
	float timeuntilbreak;
	void start() {
		pickaxe = nullptr;
	
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

		if (ismenuopen())
		{
			return false;
		}
		return true;
	}
	


	void wearduribilty() {
	
		if (pickaxe!= nullptr)
		{
			if (pickaxe->itemtype == wear) {
				pickaxe->amt -= 1;
			}
		}
	}
	

	

	void testifmining()
		{
			if (currmining != nullptr)
			{
				
				if (currmining == ((block*)(closest.box->owner)))
				{
					
					int pickpower=1;
					if (pickaxe!=nullptr)
					{
						pickpower = pickaxe->properties.pickaxepower;
					}
					if (pickpower>=currmining->mininglevel||!currmining->minedfastwithpick)
					{

						float timemod = pickpower / (1 + currmining->mininglevel);
						timeuntilbreak -= timemod*timename::dt;
						return;
					}
					
					
				}
			}
			currmining = (block*)(closest.box->owner);
			if (currmining!=nullptr)
			{

				timeuntilbreak = currmining->mininglevel;

			}
	}



	void destroylogic() {
	
		testifmining();
		if (timeuntilbreak<=0)
		{
			timeuntilbreak = currmining->mininglevel;
			wearduribilty();
			gridutil::setblock(toblk(closest.box->owner).pos, minecraftair);

		}
	}
	 void update() {

		

		pickaxe = owner->getcomponent<inventory>().selected;

		ray cameraray = ray(Vector3(camera::campos), Vector3(camera::campos) + camera::direction() * 7);
		closest = collision::raycastall(cameraray,owner,voxtra::countsolid);
		if (!caninteract())
		{
			return;
		}



		if (userinput::mouseleft.held)
		{


			destroylogic();
		}
	}
};


#endif // ! playerplace_H
