#include "particles.h"

void particleemiter::update()
{
	timetillspawn -= timename::dt;
	while (particlequeue.peek()->getcomponent<particle>().endtime< timename::gametime)
	{
		entity::entity* todelete = particlequeue.pop();
		gameobject::destroy(todelete);
	
	}

	if (shouldspawnparticle())
	{
		entity::entityref newparticle = entity::createentity(position, "");

		newparticle.toent()->addcomponent<particle>();
		 (*particleinit)(newparticle.toent());

	}
	
}

particleemiter::particleemiter(float spawntime, void (*initfunc) (entity::entity*))
{
	particlespawntime = spawntime;
	particleinit = initfunc;
	timetillspawn = spawntime;
}

bool particleemiter::shouldspawnparticle()
{
	if (timetillspawn<0)
	{
		timetillspawn = particlespawntime;
		return true;
	}
	return false;
}
