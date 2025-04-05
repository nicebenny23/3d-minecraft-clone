#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/entityutil.h"
#pragma once
struct playercam :gameobject::component
{
	void start() {
		priority = 10000;
	}
	void update() {



		ray cameraray = ray( toent(owner).transform.position, toent(owner).transform.position +toent(owner).transform.getnormaldirection()* interactmaxrange);
		voxtra::RayWorldIntersection closest = collision::raycastall(cameraray);
		if (closest.intersected())
		{
			for (int i = 0; i < closest.collider->owner->componentlist.length; i++) {
				closest.collider->owner->componentlist[i]->onplayerclick();

			}
		}
	}
};

