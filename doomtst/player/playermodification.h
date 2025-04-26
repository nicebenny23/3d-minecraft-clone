#include "../renderer/uibox.h"
#include "../game/gameobject.h"
#include "../game/collision.h"
#include "../world/managegrid.h"
#include "../game/objecthelper.h"
#include "../world/voxeltraversal.h"
#include "../util/dynamicarray.h"
#include "../items/Container.h"
#include "playerinventory.h"
#include "../entities/onhit.h"
#include "../items/itemutil.h"
#include "../game/particles.h"
using namespace objutil;
#ifndef  playerplace_Hpp
#define playerplace_Hpp
void initbreakparticle(Ent::entity* newent);
struct playerbreak:gameobject::component
{
	
	voxtra::RayWorldIntersection closest;
	item* pickaxe;
	block* currmining;
	
	float timeuntilbreak;
	void start() {
		pickaxe = nullptr;
	
		closest.collider=nullptr;
		 renderer::Ren.Textures.LoadTexture("images\\menutex.png","MenuTexture");
	
	}
	bool caninteract() {

	


		if (closest.collider == nullptr)
		{
			return false;
		}
		if (closest.collider->owner->type!=gameobject::block)
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
				
				if (currmining == ((block*)(closest.collider->owner)))
				{
				


					int pickpower=1;
					if (pickaxe!=nullptr)
					{
						pickpower = pickaxe->properties.pickaxepower;
					}
					if (pickpower>=currmining->mininglevel||!currmining->minedfastwithpick)
					{

						float timemod = pickpower / (1 + currmining->mininglevel);
						timeuntilbreak -= timemod*CtxName::ctx.Time->dt;
						return;
					}
					
					
				}
			}
			currmining = (block*)(closest.collider->owner);
			if (currmining!=nullptr)
			{

				timeuntilbreak = currmining->mininglevel/5.f;

			}
	}



	void destroylogic() {
		
		testifmining();
		if (timeuntilbreak<=0)
		{
			timeuntilbreak = currmining->mininglevel/5.f;
			wearduribilty();
			objutil::toblk(closest.collider->owner).bstate.broken = true; 
			gridutil::setblock(toblk(closest.collider->owner).pos, minecraftair);

		}
	}
	 void update() {
		 
		 
		pickaxe = owner->getcomponent<inventory>().selected;

		ray cameraray = ray(toent( owner).transform.position, toent(owner).transform.position+ toent(owner).transform.getnormaldirection()*7);
		closest = collision::raycastall(cameraray,owner,voxtra::countsolid);
		if (!caninteract())
		{
			return;
		}



		if (CtxName::ctx.Inp->mouseleft().held)
		{


			destroylogic();
		}
	}
};


#endif // ! playerplace_H
