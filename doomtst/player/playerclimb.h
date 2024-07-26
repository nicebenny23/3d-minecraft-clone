#include "../game/gameobject.h"
#include "../game/rigidbody.h"
#include "../game/objecthelper.h"
#ifndef playerclimb_HPP
#define playerclimb_HPP
struct playerclimb:gameobject::component
{

	bool onrope;

	playerclimb() {
		priority = 111;
		onrope = false;
	}	
	void oncollision(gameobject::obj* collidedwith) {
		if (collidedwith->type==gameobject::block)
		{
			blockname::block potrope =objutil::toblk(collidedwith);
			if (potrope.id==minecraftrope)
			{
				onrope = true;
				owner->getcomponent<rigidbody>().gravityenabled = false;
			}
		}
	}


	void update() {
		if (onrope==false)
		{

			owner->getcomponent<rigidbody>().gravityenabled = true;

		}onrope = false;
	}
};

#endif // !playerclimb_H
