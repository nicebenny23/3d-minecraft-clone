#include "particles.h"

void particleemiter::update()
{
	timetillspawn -= timename::dt;
	for (int i = 0; i < particlearray.length; i++)
	{
		if (particlearray[i]->getcomponent<particle>().endtime < timename::gametime)
		{
			entityname::entity* todelete = particlearray[i];
			gameobject::destroy(todelete);

		}
	}


	if (shouldspawnparticle())
	{
		entityname::entityref newparticle = entityname::createentity(position, "");

		newparticle.toent()->addcomponent<particle>();
		 (*particleinit)(newparticle.toent());

	}
	
}

void particleemiter::renderparticles()
{
	
}

particleemiter::particleemiter(float spawntime, void (*initfunc) (entityname::entity*))
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
