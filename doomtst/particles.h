#include "util/vector3.h"
#include "game/gameobject.h"
#include "renderer/texture.h"
#include "util/dynamicarray.h"
#include "util/queue.h"
#include "game/entity.h"
#include "util/time.h"
#include "renderer/renderer.h"
#include "renderer/vertexobject.h"
#ifndef  particle_HPP
#define particle_HPP

using namespace v3;
using namespace dynamicarray;
struct particleemiter:gameobject::component
{

	v3::Vector3 position;
	texture tex;
	int maxparticles = 100;
	vobj::vao emitervoa;
	//tiny quad
	vobj::vbuf emmitervbo;
	vobj::vbuf emmiteribo;
	array<entityname::entity*> particlearray;
	float particlespawntime;
	float timetillspawn;
	void (*particleinit) (entityname::entity*);
	particleemiter(float spawntime, void (*initfunc) (entityname::entity*));
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
