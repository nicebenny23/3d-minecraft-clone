#include "../game/objecthelper.h"
#include "../game/rigidbody.h"
#pragma once 
struct rotatetwordsvel:gameobject::component
{
	void update() {

		Vector3 vel = owner->getcomponent<rigidbody>().velocity;
		vel= normal(vel);
		owner->transform().OrientDir(vel);
	}
};


// !
