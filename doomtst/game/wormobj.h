
#include "aabb.h"
#include <glm/mat4x4.hpp>
#include "../player/player.h"
#include "../renderer/model.h"
#pragma once 
inline float stn::lerpfloat(float f1, float f2, float lval) {

	return f1 * (1 - lval) + f2 * lval;
}
struct worm: ecs::component
{
	float magoffset;
	int length;
	worm(int len,float offset){
		magoffset = offset;
		length = len;
	}
	void update() {
		
	
		updateglobals();
	}
	void updateglobals() {
		Transform currtransform= owner().get_component<model>()[0].transform;
		float speed = .01;
		float stn::lerpv = .01;
		aabb::Collider& col = owner().get_component<aabb::Collider>();
	

			ModelMeshName::ModelMesh* meshatpos= owner().get_component<model>().meshlist[i];
			Transform& local = col.get_component<ecs::transform_comp>().transform;
			local.position += currtransform.normal_dir() * speed;

			
			Transform shouldbetransform = currtransform;
          shouldbetransform.position-=currtransform.normal_dir()* magoffset;
         local.pitch = stn::lerpfloat(local.pitch, shouldbetransform.pitch, stn::lerpv);
         local.yaw = stn::lerpfloat(local.yaw, shouldbetransform.yaw, stn::lerpv);
		 local.position = stn::lerp(local.position, shouldbetransform.position, stn::lerpv);
		 currtransform = owner().get_component<model>()[i].transform;
		}

		
	

};

