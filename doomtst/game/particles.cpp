#include "particles.h"
#include "rigidbody.h"
#include "../world/noise.h"
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
		if (particlearray[i] != nullptr)
		{


			if (particlearray[i]->hascomponent<particle>())
			{


				if (particlearray[i]->getcomponent<particle>().endtime < CtxName::ctx.Time->realtime)
				{
					Ent::entity* EntityDeletionBuffer = particlearray[i];
					objutil::toent(EntityDeletionBuffer).Destroy();
				
				
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
				 Ent::entityref newparticle = CtxName::ctx.EntMan->CreateEntity(position, "");
				 
				 newparticle.toent()->addcomponent<particle>()->endtime= CtxName::ctx.Time->realtime+particlelifetime;
				 newparticle->getcomponent<particle>().ind = i;
				 newparticle->getcomponent<particle>().emit = this;
				 (*particleinit)(newparticle.toent());
				 newparticle->addtag("particle");
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
	renderer::Ren.Textures.Apply(tex);
	

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	renderer::changerendertype(renderer::renderparticle);
	array<float> databuf;
	for (int ind = 0; ind < 6; ind++)
	{
		int i = indices[ind];
		Vector3 vertice = vertexlist[i];

		v3::Vector3 center = zerov;

		Vector3 rightvertex = (camera::GetCamRight())*vertice.x;

		Vector3 upvertex = (camera::GetCamUp()) * vertice.y ;
		vertice = upvertex + rightvertex + center;
			databuf.append(vertice.x);
		databuf.append(vertice.y);
		databuf.append(vertice.z);
		databuf.append(cubeuv[2 * i]);

		databuf.append(cubeuv[2 * i+1]);
	}
	Ren.Fill(&ParticleMesh, databuf);
	
	for (int i = 0; i < particlearray.length; i++)
	{

		if (particlearray[i]!=nullptr)
		{
			Ren.CurrentShader()->SetVector3f(particlearray[i]->transform.position.glm(), "offset");
			Ren.CurrentShader()->SetVector3f(particlearray[i]->transform.scale.glm(), "scale");
			Ren.Render(&ParticleMesh);
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(0);
	//databuf.destroy();
}
void initbaseparticle(Ent::entity* newent) {
	newent->transform.position += Vector3(random(), 1, random())/10;
	newent->addcomponent<aabb::Collider>(newent->transform.position, unitv / 9, true);
	newent->addcomponent<rigidbody>(1,.1)->velocity=Vector3(random(),1,random())*2;
	newent->transform.scale = blockscale / 22;
	
}
particleemiter::particleemiter(float spawntime,float lifetime, void (*initfunc) (Ent::entity*),Texture2D* newtex)
{
	tex = newtex;
	
	Ren.Gen<false>(&ParticleMesh);
	ParticleMesh.AddAttribute<float, 3>().AddAttribute<float, 2>();
	particlelifetime = lifetime;
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

