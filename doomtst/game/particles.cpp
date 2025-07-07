#pragma once
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
	timetillspawn = Min(timetillspawn-CtxName::ctx.Time->dt,particlespawntime);
	for (int i = 0; i < particlearray.length; i++)
	{
		if (particlearray[i].Id !=Ids::None)
		{


			if (particlearray[i].hascomponent<particle>())
			{


				if (particlearray[i].getcomponent<particle>().endtime < CtxName::ctx.Time->ElapsedTime)
				{
					gameobject::obj EntityDeletionBuffer = particlearray[i];
					EntityDeletionBuffer.immediate_destroy();
				
				
					particlearray[i].Id =Ids::None;
				}
			}
		}
	}


	if (shouldspawnparticle())
	{
		
		 for (size_t i = 0; i < maxparticles; i++)
		 {
			 if (particlearray[i].Id ==Ids::None)
			 {
				 gameobject::obj newparticle = CtxName::ctx.OC->CreateEntity(position);
				 
				 newparticle.addcomponent<particle>()->endtime= CtxName::ctx.Time->ElapsedTime+particlelifetime;
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
		Vector3 vertice = vertexlist[i];

		v3::Vector3 center = zerov;

		Vector3 rightvertex = (camera::GetCamRight())*vertice.x;

		Vector3 upvertex = (camera::GetCamUp()) * vertice.y ;
		vertice = upvertex + rightvertex + center;
			databuf.push(vertice.x);
		databuf.push(vertice.y);
		databuf.push(vertice.z);
		databuf.push(cubeuv[2 * i]);

		databuf.push(cubeuv[2 * i+1]);
	}
	CtxName::ctx.Ren->Fill(&ParticleMesh, databuf);
	
	for (int i = 0; i < particlearray.length; i++)
	{

		if (particlearray[i].Id !=Ids::None)
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
	newent.transform().position += Vector3(random(), 1, random()) / 10;
	newent.addcomponent<aabb::Collider>(newent.transform().position, unitv / 9, true);
	newent.addcomponent<rigidbody>(1, .1)->velocity = Vector3(random(), 1, random()) * 2;
	newent.transform().scale = blockscale / 22;
	
}
particleemiter::particleemiter(float spawntime,float lifetime, void (*initfunc) (gameobject::obj),Texture2D* newtex)
{
	tex = newtex;
	
	CtxName::ctx.Ren->Gen<false>(&ParticleMesh);
	ParticleMesh.AddAttribute<float, 3>().AddAttribute<float, 2>();
	particlelifetime = lifetime;
	particlespawntime = spawntime;
	particleinit = initfunc;
	timetillspawn = spawntime;
	for ( int i = 0; i < maxparticles; i++)
	{
		particlearray[i].Id = Ids::None;
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

