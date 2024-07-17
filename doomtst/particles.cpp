#include "particles.h"
using namespace renderer;
Vector3 vertexlist[]{
Vector3(-1, -1,0),
Vector3(1, -1,0),

Vector3(-1, 1,0),
Vector3(1, 1,0)
};
const float cubeuv[] = {
	0, 1,
	1, 1,

0, 0,
1, 0

}; 
const int indices[]{
0,1,3,0,3,2
};
void particleemiter::update()
{
	timetillspawn -= timename::dt;
	for (int i = 0; i < particlearray.length; i++)
	{
		if (particlearray[i]->getcomponent<particle>().endtime < timename::gametime)
		{
			entityname::entity* todelete = particlearray[i];
			gameobject::destroy(todelete);
			particlearray[i] = nullptr;
		}
	}


	if (shouldspawnparticle())
	{
		entityname::entityref newparticle = entityname::createentity(position, "");

		newparticle.toent()->addcomponent<particle>();
		 (*particleinit)(newparticle.toent());
		 for (size_t i = 0; i < maxparticles; i++)
		 {
			 if (particlearray[i] == nullptr)
			 {
				 particlearray[i] = newparticle.toent();
			 };

		 }

	}
	
}

void particleemiter::renderparticles()
{
	tex.apply();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	renderer::changerendertype(renderer::renderparticle);
	array<float> databuf;
	for (int i = 0; i < 4; i++)
	{
		Vector3 vertice = vertexlist[i]+position;
		databuf.append(vertice.x);
		databuf.append(vertice.y);
		databuf.append(vertice.z);
		databuf.append(cubeuv[2 * i]);

		databuf.append(cubeuv[2 * i+1]);
	}
	emmitervbo.bind();

	emmitervbo.fillbuffer<float>(databuf);
	array<unsigned int> indicebuf;

	for (int i = 0; i < 6; i++)
	{
		indicebuf.append(indices[i]);
	}
	emmiteribo.bind();

	emmiteribo.fillbuffer<unsigned int>(indicebuf);
	for (int i = 0; i < particlearray.length; i++)
	{

		if (particlearray[i]!=nullptr)
		{
			renderer::shaderlist[particleshader].SetVector3f(  particlearray[i]->pos.glm(), "offset");
			
			emitervoa.bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

particleemiter::particleemiter(float spawntime, void (*initfunc) (entityname::entity*))
{
	emitervoa.generate();
	emmitervbo.generate(GL_ARRAY_BUFFER);
	emmiteribo.generate(GL_ELEMENT_ARRAY_BUFFER);
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
