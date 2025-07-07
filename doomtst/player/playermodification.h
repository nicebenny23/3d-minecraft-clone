#include "../renderer/uibox.h"
#include "../game/gameobject.h"
#include "../game/collision.h"
#include "../world/managegrid.h"
#include "../game/objecthelper.h"
#include "../world/voxeltraversal.h"
#include "../util/dynamicarray.h"
#include "../debugger/debug.h"
#include "playerinventory.h"
#include "../items/itemutil.h"
#pragma once 
void initbreakparticle(gameobject::obj newent);
struct playerbreak: gameobject::component
{
	
	voxtra::WorldRayCollision closest;
	item* pickaxe;
	block* currmining;
	
	float timeuntilbreak;
	void start() {
		pickaxe = nullptr;
	
		 CtxName::ctx.Ren->Textures.LoadTexture("images\\menutex.png","MenuTexture");
	
	}
	bool caninteract() {

	


		if (!closest)
		{
			return false;
		}
		voxtra::RayWorldHit Hit = *closest;
		if (!Hit.collider->owner.hascomponent<blockname::block>())
		{
			return false;
		}
		debug(float( Hit.collider->owner.getcomponent<blockname::block>().id));
		if (!inrange(Hit.Dist(), interactminrange, interactmaxrange))
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
		voxtra::RayWorldHit Hit = closest.unwrap();
			if (currmining != nullptr)
			{
				
				if (currmining == Hit.collider->owner.getcomponentptr<block>())
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
			currmining = Hit.collider->owner.getcomponentptr<block>();
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
			voxtra::RayWorldHit Hit = closest.unwrap();
			objutil::toblk(Hit.collider->owner).bstate.broken = true; 
			gridutil::setblock(objutil::toblk(Hit.collider->owner).pos, minecraftair);

		}
	}
	 void update() {
		 
		 
		pickaxe = owner.getcomponent<inventory>().selected;

		ray cameraray = ray(owner.transform().position,owner.transform().position+owner.transform().getnormaldirection()*7);
		closest = collision::raycastall(cameraray, collision::HitQuery(owner),voxtra::countsolid);
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


 // ! playerplace_H
