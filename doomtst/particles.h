#include "util/vector3.h"
#include "game/gameobject.h"
#include "renderer/texture.h"
#include "util/dynamicarray.h"
#include "util/queue.h"
#include "game/entity.h"
using namespace v3;
using namespace dynamicarray;
struct particleemmiter:gameobject::component
{





	queuename::queue<entity::entity*> particlequeue;
	void(*entity::entity);
	bool shouldspawnparticle();
	void update() {
		while (particlequeue.peek()->getcomponent<particle>().timeleft<0)
		{
			entity::entity* todelete = particlequeue.pop();
			gameobject::destroy(todelete);
		}

	//	if (shouldspawn)
		{

		}

    }

};


struct particle:gameobject::component
{
	
	
	float timeleft;
	

	particle() = default;
};