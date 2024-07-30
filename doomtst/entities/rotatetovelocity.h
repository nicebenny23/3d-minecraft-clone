#include "../game/objecthelper.h"
#include "../game/rigidbody.h"
#ifndef velrot_HPP
#define velrot_HPP
struct rotatetwordsvel:gameobject::component
{
	void update() {

		Vector3 vel = owner->getcomponent<rigidbody>().velocity;
		vel= normal(vel);
		objutil::toent(owner).transform.orientbase(vel);
	}
};

#endif // !
