#include "../game/gameobject.h"
#include "../game/rigidbody.h"
#include "../game/objecthelper.h"
#pragma once 
struct playerclimb: gameobject::component
{

	bool onrope;

	playerclimb() {
		priority = 111;
		onrope = false;
	}	
	void oncollision(gameobject::obj* collidedwith) {
		if (collidedwith->hascomponent<blockname::block>())
		{
			blockname::block* potrope =&objutil::toblk(collidedwith);
			if (potrope->id==minecraftrope)
			{
				onrope = true;
				owner.getcomponent<rigidbody>().gravityscale= 0;
			}
		}
	}


	void update() {
		if (onrope==false)
		{

			owner.getcomponent<rigidbody>().gravityscale = 1;

		}onrope = false;
	}
};

 // !playerclimb_H
