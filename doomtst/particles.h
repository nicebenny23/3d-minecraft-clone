#include "util/vector3.h"
#include "game/gameobject.h"
#include "renderer/texture.h"
#include "util/dynamicarray.h"
#include "util/queue.h"
#include "game/entity.h"
#include "util/time.h"
#include "renderer/vertexobject.h"
#ifndef  particle_HPP
#define particle_HPP

using namespace v3;
using namespace dynamicarray;
struct particleemiter:gameobject::component
{

	v3::Vector3 position;
	
	

	queuename::queue<entity::entity*> particlequeue;
	float particlespawntime;
	float timetillspawn;
	void (*particleinit) (entity::entity*);
	particleemiter(float spawntime, void (*initfunc) (entity::entity*));
	bool shouldspawnparticle();
	void update();

};


struct particle:gameobject::component
{
	
	
	float endtime;
	

	particle() = default;
}; 
#endif // ! particle_HPP
