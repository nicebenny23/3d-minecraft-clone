#include "particles.h"
#include "rigidbody.h"
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
		if (particlearray[i] != nullptr)
		{


			if (particlearray[i]->hascomponent<particle>())
			{


				if (particlearray[i]->getcomponent<particle>().endtime > timename::gametime)
				{
					entityname::entity* todelete = particlearray[i];
					entityname::destroy(todelete);
				
				
					particlearray[i] = nullptr;
				}
			}
		}
	}


	if (shouldspawnparticle())
	{
		
		 for (size_t i = 0; i < maxparticles; i++)
		 {
			 if (particlearray[i] == nullptr)
			 {
				 entityname::entityref newparticle = entityname::createentity(position, "");

				 newparticle.toent()->addcomponent<particle>();
				 (*particleinit)(newparticle.toent());
				 particlearray[i] = newparticle.toent();
				 break;
			 };

		 }

	}
	
}

void particleemiter::start()
{
	position = objutil::toent(owner).transform.position;
}

void particleemiter::renderparticles()
{
	tex.apply();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	renderer::changerendertype(renderer::renderparticle);
	array<float> databuf;
	for (int ind = 0; ind < 6; ind++)
	{
		int i = indices[ind];
		Vector3 vertice = vertexlist[i];

		v3::Vector3 center = position;

		Vector3 rightvertex = (camera::rightvec)*vertice.x;

		Vector3 upvertex = (camera::upvec) * vertice.y ;
		vertice = upvertex + rightvertex + center;
			databuf.append(vertice.x);
		databuf.append(vertice.y);
		databuf.append(vertice.z);
		databuf.append(cubeuv[2 * i]);

		databuf.append(cubeuv[2 * i+1]);
	}
	emmitervbo.bind();

	emmitervbo.fillbuffer<float>(databuf);
	emitervoa.bind();        
	emitervoa.set_attr(0, 3, GL_FLOAT, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	//texture coords,inclusing the texture in the array 
	emitervoa.set_attr(1, 2, GL_FLOAT, 5* sizeof(float), 3 * sizeof(float));
	glEnableVertexAttribArray(1);


	for (int i = 0; i < particlearray.length; i++)
	{

		if (particlearray[i]!=nullptr)
		{
			renderer::shaderlist[particleshader].SetVector3f(  particlearray[i]->transform.position.glm(), "offset");
			
			glDrawArrays(GL_TRIANGLES, 0, 6);
			}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(0);
	//databuf.destroy();
}
void initbaseparticle(entityname::entity* newent) {
	//newent->addcomponent<aabb::Collider>(newent->pos, unitv / 2, true);


}
particleemiter::particleemiter(float spawntime, void (*initfunc) (entityname::entity*))
{
	emitervoa.generate();
	emmitervbo.generate(GL_ARRAY_BUFFER);

	particlespawntime = spawntime;
	particleinit = initfunc;
	timetillspawn = spawntime;
	for ( int i = 0; i < maxparticles; i++)
	{
		particlearray[i] = nullptr;
	}
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

