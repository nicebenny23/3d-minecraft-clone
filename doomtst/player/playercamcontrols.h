#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/entityutil.h"
#include "../items/menu.h"
#include "../game/query.h"
#pragma once
struct playercamcontrols : gameobject::component
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

			owner.transform().pitch += yoffset;

			owner.transform().yaw += xoffset;

			owner.transform().pitch = clamp(owner.transform().pitch, -89.99, 89.99);

		}
		else
		{
			CtxName::ctx.Window->EnableCursor();
		}
		ray cameraray = ray(owner.transform().position,owner.transform().position +owner.transform().getnormaldirection() * interactmaxrange);
		voxtra::WorldRayCollision closest = collision::raycastall(cameraray, collision::HitQuery());
		if (closest)
		{
			for (component* comp:query::ComponentView(closest.unwrap().collider->owner)) {
				comp->onplayerclick();
			}
		}

	}
};
