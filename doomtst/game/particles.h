#pragma once
#include "../util/vector3.h"
#include "../game/gameobject.h"
#include "../renderer/texture.h"
#include "../util/dynamicarray.h"
#include "../util/queue.h"
#include "../game/entity.h"
#include "../util/time.h"
#include "../renderer/renderer.h"
#include "camera.h"

using namespace v3;
using namespace Cont;
struct particleemiter:gameobject::component
{

	v3::Vector3 position;
	Texture2D* tex;
	int maxparticles = 1000;
	Mesh ParticleMesh;
	array<Ent::entity*> particlearray;
	float particlespawntime;
	float particlelifetime;
	float timetillspawn;
	void (*particleinit) (Ent::entity*);
	particleemiter(float spawntime,float lifetime, void (*initfunc) (Ent::entity*),Texture2D* newtex);
	bool shouldspawnparticle();
	void update();
	void start();
	void renderupdate() {

		renderparticles();
	}
	void renderparticles();
};
void initbaseparticle(Ent::entity* newent);

struct particle:gameobject::component
{
	
	
	float endtime;
	int ind;
	particleemiter* emit;
	~particle() {

		emit->particlearray[ind] = nullptr;
	}
	particle() = default;
}; 
