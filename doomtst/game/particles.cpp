#pragma once
#include "particles.h"
#include "rigidbody.h"
using namespace renderer;
Vec3 vertexlist[]{
Vec3(-1, -1,0),
Vec3(1, -1,0),

Vec3(-1, 1,0),
Vec3(1, 1,0)
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
	timetillspawn = Min(timetillspawn-CtxName::ctx.Time->dt,particlespawntime);
	for (int i = 0; i < particlearray.length(); i++)
	{
		if (particlearray[i].exists())
		{


			if (particlearray[i].hascomponent<particle>())
			{


				if (particlearray[i].getcomponent<particle>().endtime < CtxName::ctx.Time->now().value)
				{
					gameobject::obj EntityDeletionBuffer = particlearray[i];
					EntityDeletionBuffer.destroy();
				
				
					//particlearray[i].GenId =Ids::NoneG;
				}
			}
		}
	}


	if (shouldspawnparticle())
	{
		
		 for (size_t i = 0; i < maxparticles; i++)
		 {
			 if (particlearray[i].exists())
			 {
				 gameobject::obj newparticle = CtxName::ctx.OC->spawn_with_transform(position);
				 
				 newparticle.addcomponent<particle>()->endtime= CtxName::ctx.Time->now().value +particlelifetime;
				 newparticle.getcomponent<particle>().ind = i;
				 newparticle.getcomponent<particle>().emit = this;
				 (*particleinit)(newparticle);
				
				 particlearray[i] = newparticle;
				 break;
			 };

		 }

	}
	
}

void particleemiter::start()
{
	position = owner.transform().position;
}

void particleemiter::renderparticles()
{
	CtxName::ctx.Ren->context.Bind(*tex);
	

	glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
	CtxName::ctx.Ren->SetType("Particle");
	array<float> databuf;
	for (int ind = 0; ind < 6; ind++)
	{
		int i = indices[ind];
		Vec3 vertice = vertexlist[i];

		v3::Vec3 center = zerov;

		Vec3 rightvertex = (camera::GetCamRight())*vertice.x;

		Vec3 upvertex = (camera::GetCamUp()) * vertice.y ;
		vertice = upvertex + rightvertex + center;
			databuf.push(vertice.x);
		databuf.push(vertice.y);
		databuf.push(vertice.z);
		databuf.push(cubeuv[2 * i]);

		databuf.push(cubeuv[2 * i+1]);
	}
	CtxName::ctx.Ren->Fill(&ParticleMesh, databuf);
	
	for (int i = 0; i < particlearray.length(); i++)
	{

		if (particlearray[i].exists())
		{
			CtxName::ctx.Ren->CurrentShader()->SetVector3f(particlearray[i].transform().position.glm(), "offset");
			CtxName::ctx.Ren->CurrentShader()->SetVector3f(particlearray[i].transform().scale.glm(), "scale");
			CtxName::ctx.Ren->Render(&ParticleMesh);
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(0);
	//databuf.destroy();
}
void initbaseparticle(gameobject::obj newent) {
	newent.transform().position += Vec3(random(), 1, random()) / 10;
	newent.addcomponent<aabb::Collider>(newent.transform().position, unitv / 9, true);
	newent.addcomponent<rigidbody>(1, .1)->velocity = Vec3(random(), 1, random()) * 2;
	newent.transform().scale = blockscale / 22;
	
}
particleemiter::particleemiter(float spawntime,float lifetime, void (*initfunc) (gameobject::obj),Texture2D* newtex)
{
	tex = newtex;
	
	CtxName::ctx.Ren->Gen(&ParticleMesh);
	ParticleMesh.Voa.attributes.push<float, 3>().push<float, 2>();
	particlelifetime = lifetime;
	particlespawntime = spawntime;
	particleinit = initfunc;
	timetillspawn = spawntime;
	for ( int i = 0; i < maxparticles; i++)
	{
		//particlearray[i].GenId = Ids::NoneG;
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

