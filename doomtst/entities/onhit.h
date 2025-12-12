#include "../game/entity.h"
#include "../game/collision.h"
#include "../player/playerhealth.h"
#include "../game/rigidbody.h"
#include "../game/entityutil.h"
#pragma once 
template<typename T,bool is_not_tag>
struct destroyonhit : ecs::component{

	

	destroyonhit() {
	}
	void oncollision(ecs::obj collidedwith) {

		
		
			if (collidedwith.has_component<T>()^is_not_tag)
			{
				owner().destroy();
			}
		
	}

	~destroyonhit() = default;
};
template<typename T>

struct dmgonhit: ecs::component
{

	
	float knockback;

	int dmgdone;
	dmgonhit(int dmg,float kb=0) {
	dmgdone = dmg;
	knockback = kb;
	
	}
	void oncollision(ecs::obj collidedwith) {

		
			if (!collidedwith.has_component<estate>()) {
			
				return;
			}
			if (!(collidedwith.has_component<T>()))
			{
				return;
			}
			if (collidedwith.has_component < ecs::transform_comp>())
			{


				v3::Vec3 center = owner().get_component<ecs::transform_comp>().transform.position;
				v3::Vec3 othercenter = collidedwith.get_component<ecs::transform_comp>().transform.position;
				if (collidedwith.has_component<rigidbody>()) {


					kb(center, knockback, collidedwith);



				}
				collidedwith.get_component<estate>().damage(dmgdone);
			}
		
	}
	~dmgonhit() = default;
};

 // !properties.dmgonhit_HPP
namespace ecs {
	template<typename T, bool b>
	inline constexpr ComponentInfo ComponentTraits<destroyonhit<T, b>> = {
		.updates = false
	};
	template<typename T>
	inline constexpr ComponentInfo ComponentTraits<dmgonhit<T>> = {
		.updates = false
	};
}