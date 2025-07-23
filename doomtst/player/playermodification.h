#include "../renderer/uibox.h"
#include "../game/gameobject.h"
#include "../game/collision.h"
#include "../world/managegrid.h"
#include "../game/objecthelper.h"
#include "../world/voxeltraversal.h"
#include "../util/dynamicarray.h"
#include "../debugger/debug.h"
#include "playerinventory.h"
#include "../items/itemutil.h"
#include "../renderer/decal.h"
#include "../game/System.h"
#pragma once 
//assumtion dot product is negitive or psotive for nonzero so one of them must be positive thus it will alway ewxist
inline Dir::Dir3d closest_face(v3::Vec3 pos, block* blk) {
	pos-= blk->center();
	
	Dir::Dir3d max_product=Dir::up3d;
	for (Dir::Dir3d vec: Dir::Directions3d )
	{
		if (dot(max_product.ToVec(), pos) < dot(vec.ToVec(), pos)) {
			max_product = vec;
		}
	}
	return max_product;
}
inline util::pair<v3::Vec3,v3::Vec3> get_flat_frame(Dir::Dir3d direction) {
	size_t ind = direction.ind()/2;
	switch (ind)
	{
	case 0:
		return util::pair(v3::Vec3(0, 0, 1), v3::Vec3(0, 1, 0));
		break;
	case 1:

		return util::pair(v3::Vec3(1, 0, 0), v3::Vec3(0, 0, 1));
		break;
	case 2:

		return util::pair(v3::Vec3(1, 0, 0), v3::Vec3(0, 1, 0));
		break;
	default:
		throw std::logic_error("How?");
		break;

	}
}
void initbreakparticle(gameobject::obj newent);
struct playerbreak: gameobject::component
{
	
	voxtra::WorldRayCollision closest;
	item* pickaxe;
	float break_start_time;
	block* currmining;
	decal break_decal;
	float timeuntilbreak;
	void start() {
		pickaxe = nullptr;
	
		 CtxName::ctx.Ren->Textures.LoadTexture("images\\menutex.png","MenuTexture");

	}
	void spawn_decal(size_t phase) {
		if (phase==0)
		{
			return;
		}
		if (break_decal.handle())
		{
			break_decal.destroy();
		
			
		}std::string name;
		std::string handle;

		switch (phase) {
		case 0:
			return;
			break;
		case 1:
			name = "images\\block_break_1.png";
			handle = "block_break_1";
			break;
		case 2:
			name = "images\\block_break_2.png";
			handle = "block_break_2";
			break;
		case 3:
			name = "images\\block_break_3.png";
			handle = "block_break_3";
			break;
		case 4:
			name = "images\\block_break_4.png";
			handle = "block_break_4";
			break;
		case 5:
			name = "images\\block_break_5.png";
			handle = "block_break_5";
			break;
		case 6:
			name = "images\\block_break_6.png";
			handle = "block_break_6";
			break;
		default:
			// Optional: handle unexpected values
			throw std::logic_error("invalid block break state");
			break;
		}

		break_decal.create_handle(name.c_str(), handle.c_str());
		Dir::Dir3d cface = closest_face(closest.unwrap().Hit.intersectionpoint, currmining);
		face& opt_face = currmining->mesh[cface.ind()];
		break_decal.center = opt_face.center();
		break_decal.normal = cface.ToVec();
		auto [bitan, tan] = get_flat_frame(cface);
		break_decal.bi_tangent = bitan/2;

		break_decal.tangent = tan/2;
		break_decal.create_mesh();
		break_decal.render();
	}
	bool caninteract() {

	


		if (!closest)
		{
			return false;
		}
		voxtra::RayWorldHit Hit = *closest;
		if (!Hit.collider->owner.hascomponent<blockname::block>())
		{
			return false;
		}
	
		if (!inrange(Hit.Dist(), interactminrange, interactmaxrange))
		{
			return false;
		}

		if (ismenuopen())
		{
			return false;
		}
		return true;
	}
	


	void wearduribilty() {
	
		if (pickaxe!= nullptr)
		{
			if (pickaxe->itemtype == wear) {
				pickaxe->amt -= 1;
			}
		}
	}
	

	

	void testifmining()
		{
		voxtra::RayWorldHit Hit = closest.unwrap();
			if (currmining != nullptr&& currmining == Hit.collider->owner.getcomponentptr<block>())
			{
				
				


					int pickpower=1;
					if (pickaxe!=nullptr)
					{
						pickpower = pickaxe->properties.pickaxepower;
					}
					if (pickpower>=currmining->mininglevel||!currmining->minedfastwithpick)
					{
						float completion = clamp(7*((break_start_time) - timeuntilbreak)/ (break_start_time), 0.f, 7.0f);
						size_t phase = completion;
						
							if (apx(Hit.collider->owner.getcomponent<block>().mesh.box.scale, blockscale))
							{
								spawn_decal(phase);
							}


						
						float timemod = pickpower / (1 + currmining->mininglevel);
						timeuntilbreak -= timemod*CtxName::ctx.Time->dt;
						return;
					}
					
					
				
			}
			currmining = Hit.collider->owner.getcomponentptr<block>();
			
			if (currmining!=nullptr)
			{
				break_start_time = currmining->mininglevel / 5.f;

				timeuntilbreak = break_start_time;
				
			}
	}
	void make_drop(voxtra::RayWorldHit Hit);


	void destroylogic() {
		
		testifmining();
		if (timeuntilbreak<=0)
		{
			break_start_time = currmining->mininglevel / 5.f;

			timeuntilbreak = break_start_time;
			wearduribilty();
			voxtra::RayWorldHit Hit = closest.unwrap();
	
			
		
			objutil::toblk(Hit.collider->owner).bstate.broken = true; 
			make_drop(Hit);
			gridutil::setblock(objutil::toblk(Hit.collider->owner).pos, minecraftair);

		}
	}
	 void update() {
		 
		 
		pickaxe = owner.getcomponent<inventory>().selected;

		ray cameraray = ray(owner.transform().position,owner.transform().position+owner.transform().getnormaldirection()*7);
		closest = collision::raycastall(cameraray, collision::HitQuery(owner),voxtra::countsolid);
		if (!caninteract())
		{
			return;
		}



		if (CtxName::ctx.Inp->mouseleft().held)
		{

			destroylogic();
		}
	}
};

struct PlayerUpdateSystem : System{
	void run(gameobject::Ecs* ecs) {
		query::View<playerbreak> rigids(ecs);
		for (auto [body] : rigids) {
			body->update();
			
		}
	}
};
 // ! playerplace_H
