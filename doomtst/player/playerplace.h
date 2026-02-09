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
		double max_interact_range = 7;

		if (!inrange(closest.Hit.dist, -1, max_interact_range)) {
			return false;
		}
		return true;
	}
	void placeblock() {
		ray cameraray = owner().get_component<ecs::transform_comp>().transform.forward_ray().dialate_from_start(7);
			
		stn::Option<block&> block_to_replace_opt = voxtra::findprevblock(cameraray, world().get_resource<grid::Grid>(), voxtra::GridTraverseMode::countnormal);
		if (!block_to_replace_opt) {
			return;
		}
		blocks::block& block_to_replace = block_to_replace_opt.unwrap();
		
		if (block_to_replace.attributes.solid) {
			return;
		}
		if (!Hit) {
			return;
		}
		voxtra::RayWorldHit closest = Hit.unwrap();

		math::Direction3d attach_direction = math::Align(closest.collider.global_box().center - block_to_replace.center());
		
		block_to_replace.mesh.direction = math::max_2d_direction(camera::campos() - closest.Hit.intersectionpoint);
		block_to_replace.mesh.attached_direction = attach_direction;
		stn::Option<blocks::block_id> blk_opt = world().get_resource<items::item_types>().from_id(select.unwrap().get_component<items::item_stack>().contained_id()).blk_id;
		if (!blk_opt) {
			return;
		}
		blocks::block_id blk = blk_opt.unwrap();
		v3::Scale3 mesh_scale=world().get_resource<BlockRegistry>().traits_for(blk).mesh.size;
//computes the ideal block for it 
		Box new_block_box = Box(block_to_replace.center(), mesh_scale);
		new_block_box.center -= attach_direction.vec() * mesh_scale.shrunk(.5f);
		new_block_box.scale *= .95;
		bool collides = collision::boxcast_dynamic(new_block_box, collision::HitQuery(world()));
		if (!collides) {

			if (select.unwrap().get_component<items::item_stack>().can_remove(1)) {
				grid::set_block(world(), block_to_replace.pos, blk);

				select.unwrap().get_component<items::item_stack>().remove(1);

			}
		}
	}


	void update() {
		select = owner().get_component<player::inventory>().selected_object();
		ray cameraray = ray(owner().get_component<ecs::transform_comp>().transform.position, owner().get_component<ecs::transform_comp>().transform.position + owner().get_component<ecs::transform_comp>().transform.normal_dir() * 7);
		Hit = collision::raycast(cameraray, collision::HitQuery(owner()));
		if (!caninteract()) {
			return;
		}
		if (world().get_resource<userinput::InputManager>().right_mouse().pressed) {
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
