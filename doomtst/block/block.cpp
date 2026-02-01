#include "../debugger/debug.h"
#include "block.h"
#include "../math/vector2.h"
#include "../game/aabb.h"

using namespace blocks;


void blocks::block::createdefaultaabb(bool effector) {
	owner().add_component<aabb::Collider>(v3::Point3(0, 0, 0), unit_scale / 2.f, false, effector);

}
