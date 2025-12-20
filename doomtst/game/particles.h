#pragma once
#include "../util/vector3.h"
#include "../game/ecs/game_object.h"
#include "../util/dynamicarray.h"
#include "../util/queue.h"
#include "../game/entity.h"
#include "../util/time.h"
#include "../renderer/renderer.h"
#include "camera.h"

using namespace v3;
using namespace stn;
struct particleemiter: ecs::component
{

	v3::Point3 position;
	Texture2D* tex;
	int maxparticles = 1000;
	Mesh ParticleMesh;
	array<ecs::obj> particlearray;
	float particlespawntime;
	float particlelifetime;
	float timetillspawn;
	void (*particleinit) (ecs::obj);
	particleemiter(float spawntime,float lifetime, void (*initfunc) (ecs::obj),Texture2D* newtex);
	bool shouldspawnparticle();
	void update();
	void start();
	void renderupdate() {

		renderparticles();
	}
	void renderparticles();
};
void initbaseparticle(ecs::obj* newent);

struct particle: ecs::component
{
	
	
	float endtime;
	int ind;
	particleemiter* emit;
	~particle() {

	//	emit->particlearray[ind].GenId=stn::NoneG;
	}
	particle() = default;
}; 
