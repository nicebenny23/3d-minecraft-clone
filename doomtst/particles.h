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
using namespace entityname;
using namespace v3;
using namespace dynamicarray;
struct particleemiter:gameobject::component
{

	v3::Vector3 position;
	
	
	vobj::vao emitervoa;
	//tiny quad
	vobj::vbuf emmitervbuf;
	array<entity*> particlearray;
	float particlespawntime;
	float timetillspawn;
	void (*particleinit) (entity*);
	particleemiter(float spawntime, void (*initfunc) (entity*));
	bool shouldspawnparticle();
	void update();
	void renderparticles();
};


struct particle:gameobject::component
{
	
	
	float endtime;
	

	particle() = default;
}; 
#endif // ! particle_HPP
