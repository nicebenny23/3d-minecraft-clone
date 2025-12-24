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
	float dt = CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().dt;
	timetillspawn = Min(timetillspawn-dt,particlespawntime);
	for (int i = 0; i < particlearray.length(); i++)
	{
		//if (particlearray[i].exists())
		{


			if (particlearray[i].has_component<particle>())
			{


				if (particlearray[i].get_component<particle>().endtime < CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().now().value)
				{
					std::move(particlearray[i]).destroy();
				}
			}
		}
	}


	if (shouldspawnparticle())
	{
		
		 for (size_t i = 0; i < maxparticles; i++)
		 {
			// if (particlearray[i])
			 {
			//	 ecs::obj newparticle = CtxName::ctx.Ecs->spawn_with_transform(position);
				 
			//	 newparticle.add_component<particle>().endtime= CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().now().value +particlelifetime;
			//	 newparticle.get_component<particle>().ind = i;
			//	 newparticle.get_component<particle>().emit = this;
				// (*particleinit)(newparticle);
				
				// particlearray[i] = newparticle;
				 break;
			 };

		 }

	}
	
}

void particleemiter::start()
{
	position = owner().get_component<ecs::transform_comp>().transform.position;
}

void particleemiter::renderparticles()
{
	CtxName::ctx.Ren->context.bind(*tex);
	

	glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
//	CtxName::ctx.Ren->SetType("Particle");
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
	//CtxName::ctx.Ren->Fill(&ParticleMesh, databuf);
	
	for (int i = 0; i < particlearray.length(); i++)
	{

		//if (particlearray[i].exists())
		{
			CtxName::ctx.Ren->CurrentShader().SetVector3f(particlearray[i].get_component<ecs::transform_comp>().transform.position.glm(), "offset");
			CtxName::ctx.Ren->CurrentShader().SetVector3f(particlearray[i].get_component<ecs::transform_comp>().transform.scale.glm(), "scale");
			CtxName::ctx.Ren->Render(&ParticleMesh);
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(0);
	//databuf.clear();
}
void initbaseparticle(ecs::obj newent) {
	newent.get_component<ecs::transform_comp>().transform.position += Vec3(random::random(), 1, random::random()) / 10;
	newent.add_component<aabb::Collider>(newent.get_component<ecs::transform_comp>().transform.position, unit_scale / 9, true);
	newent.add_component<rigidbody>(1, .1).velocity = Vec3(random::random(), 1, random::random()) * 2;
	newent.get_component<ecs::transform_comp>().transform.scale = blockscale / 22;
	
}
particleemiter::particleemiter(float spawntime,float lifetime, void (*initfunc) (ecs::obj),Texture2D* newtex)
{
	tex = newtex;
	
//	ParticleMesh=CtxName::ctx.Ren->Gen();
	ParticleMesh.Voa.attributes.push<float, 3>().push<float, 2>();
	particlelifetime = lifetime;
	particlespawntime = spawntime;
	particleinit = initfunc;
	timetillspawn = spawntime;
	for ( int i = 0; i < maxparticles; i++)
	{
		//particlearray[i].GenId = stn::NoneG;
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

