#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/entityutil.h"
#include "../renderer/Window.h"
#include "../game/entityutil.h"
#include "../items/menu.h"
#pragma once
struct playercamcontrols :gameobject::component
{
	void update() {


		if (!ismenuopen())
		{
			CtxName::ctx.Window->DisableCursor();

			float xoffset = CtxName::ctx.Inp->mouseposdt.x;
			float yoffset = -CtxName::ctx.Inp->mouseposdt.y;

			float sensitivity = 0.3;

			if (abs(xoffset) > 180)
			{
				xoffset = 0;
			}  if (abs(yoffset) > 180)
			{
				yoffset = 0;
			}
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			objutil::toent(owner).transform.pitch += yoffset;

			objutil::toent(owner).transform.yaw += xoffset;
			objutil::toent(owner).transform.pitch = clamp(objutil::toent(owner).transform.pitch, -89.99, 89.99);

		}
		else
		{
			CtxName::ctx.Window->EnableCursor();
		}
		ray cameraray = ray(toent(owner).transform.position, toent(owner).transform.position + toent(owner).transform.getnormaldirection() * interactmaxrange);
		voxtra::RayWorldIntersection closest = collision::raycastall(cameraray);
		if (closest.intersected())
		{
			for (int i = 0; i < closest.collider->owner->componentlist.length; i++) {
				closest.collider->owner->componentlist[i]->onplayerclick();

			}
		}

	}
};
