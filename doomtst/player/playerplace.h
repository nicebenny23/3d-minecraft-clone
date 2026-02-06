#pragma once
#include "../game/ecs/game_object.h"
#include "../game/collision.h"
#include "../world/managegrid.h"
#include "../game/objecthelper.h"
#include "../world/voxeltraversal.h"
#include "playerinventory.h"


struct playerplace : ecs::component {
	int curplaceid;
	voxtra::WorldRayCollision Hit;
	stn::Option<ecs::obj> select;


	float timeuntilbreak;
	void start() {
		curplaceid = 0;
	}
	bool caninteract() {

		if (!select) {
			return false;
		}


		if (!Hit) {
			return false;
		}
		voxtra::RayWorldHit closest = Hit.unwrap();
		if (!closest.owner().has_component<blocks::block>()) {
			return false;
		}
		if (!inrange(closest.Hit.dist, -1, interactmaxrange)) {
			return false;
		}
		return true;
	}
	void placeblock() {
		ray cameraray = ray(owner().get_component<ecs::transform_comp>().transform.position, owner().get_component<ecs::transform_comp>().transform.position + owner().get_component<ecs::transform_comp>().transform.normal_dir() * 7);
		block* block_to_replace = voxtra::findprevblock(cameraray, voxtra::countsolid);
		if (block_to_replace == nullptr) {
			return;
		}
		if (block_to_replace->attributes.solid) {
			return;
		}
		if (!Hit) {
			return;
		}
		voxtra::RayWorldHit closest = Hit.unwrap();
		math::Direction3d dir = math::Align(closest.collider.globalbox().center - block_to_replace->center());
		block_to_replace->mesh.direction = math::max_2d_direction(camera::campos() - closest.Hit.intersectionpoint);
		block_to_replace->mesh.attached_direction = dir;
		Box newblockbox = Box(block_to_replace->center(), blockscale);
		newblockbox.scale *= .95;
		bool collides = collision::boxCollidesWithEntity(newblockbox, collision::HitQuery());
		if (!collides) {

			stn::Option<blocks::block_id> blk = world().get_resource<items::item_types>().unwrap().from_id(select.unwrap().get_component<items::item_stack>().contained_id()).blk_id;

			if (blk && select.unwrap().get_component<items::item_stack>().can_remove(1)) {
				grid::set_block(world(), block_to_replace->pos, blk.unwrap());

				select.unwrap().get_component<items::item_stack>().remove(1);

			}
		}
	}


	void update() {
		select = owner().get_component<inventory>().selected_object();
		ray cameraray = ray(owner().get_component<ecs::transform_comp>().transform.position, owner().get_component<ecs::transform_comp>().transform.position + owner().get_component<ecs::transform_comp>().transform.normal_dir() * 7);
		Hit = collision::raycastall(cameraray, collision::HitQuery(owner()));
		if (!caninteract()) {
			return;
		}





		if (world().get_resource<userinput::InputManager>().unwrap().right_mouse().pressed) {


			placeblock();
		}

	}




};
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<playerplace> = {
		.priority = 3333333
	};
}
