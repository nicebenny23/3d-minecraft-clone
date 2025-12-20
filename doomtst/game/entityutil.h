#include "rigidbody.h"
#include "entitystate.h"
#pragma once 
inline void kb(Point3 point, float force,ecs::obj ent) {
	if (ent.has_component<estate>()&&ent.has_component<ecs::transform_comp>())
	{


		if (ent.get_component<estate>().timetilldmg <= 0)
		{


			if (ent.has_component<rigidbody>())
			{
				Vec3 forceval = ent.get_component<ecs::transform_comp>().transform.position- point;
				forceval = Vec3(forceval.x, .25, forceval.z);
				forceval = normal(forceval);
				
				forceval *= force;

				ent.get_component<rigidbody>().add_force( rigid_force(forceval*3,.333333333, "kb_force"));
			}
		}
	}
}
 // ! entityutil_HPP
