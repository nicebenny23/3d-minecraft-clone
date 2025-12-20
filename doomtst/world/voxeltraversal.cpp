#pragma once
#include "voxeltraversal.h"
using namespace blockname;
using	 namespace voxtra;
using namespace grid;
//by implementing inner search loop acuracy scales quadraticlly
//aproxomate voxel traversel algorthm,the accuracy scales linearly with time complexity,
bool counttablevoxel(block* blk, GridTraverseMode trav) {
	if ((trav == countsolid || trav == countnormal) && !blk->attributes.solid)
	{
		return false;

	}



	if (blk->owner().get_component<aabb::Collider>().effector)
	{
		return trav != countnormal;

	}
	return true;
}

bool voxtra::Boxcollwithgrid(geometry::Box Box)
{

	array<block*> BlocksInRange = CtxName::ctx.Grid->voxelinrange(Box);

	for (block* PotentialCollision : BlocksInRange)
	{

		if (PotentialCollision == nullptr)
		{
			continue;
		}

		aabb::Collider* Collider = PotentialCollision->owner().get_component_ptr<aabb::Collider>();


		if (Collider != nullptr && PotentialCollision->attributes.solid && !Collider->effector)
		{
			if (aabb::aabbboxintersect(Box, *Collider))
			{
				return true;
			}
		}



	}
	return false;




}

voxtra::WorldRayCollision  voxtra::travvox(ray nray, GridTraverseMode trav)
{
	if (nray.length() == 0)
	{
		return stn::None;
	}
	float ray_length = nray.length();
	float travel_dist = 0;
	Vec3 norm_ray = nray.dir();
	Vec3 conv_each;
	v3::Coord sgns;
	for (size_t i = 0; i < 3; i++)
	{
		//if its empty we set the direction to none
		if (norm_ray[i] == 0)
		{
			conv_each[i] = 0;
			sgns[i] = 0;
		}
		else {
			conv_each[i] = abs(1 / norm_ray[i]);
			sgns[i] = sign(norm_ray[i]);
		}

	}
	v3::Point3 pos = nray.start-(norm_ray*1e-6);

	Coord curvox = CtxName::ctx.Grid->getVoxel(pos);
	Coord Boundry;
	for (size_t i = 0; i < 3; i++)
	{
		Boundry[i] = next_boundary(pos[i], sgns[i] == 1);
	}
	while (travel_dist <= ray_length)
	{
		


		WorldRayCollision Collision = stn::None;
		for (int x = extended_range(pos.x).first; x <= extended_range(pos.x).second; ++x) {
			for (int y = extended_range(pos.y).first; y <= extended_range(pos.y).second; ++y) {
				for (int z = extended_range(pos.z).first; z <= extended_range(pos.z).second; ++z) {
					block* blk = CtxName::ctx.Grid->getBlock(Coord(x, y, z));
					if (blk == nullptr)
					{
						continue;
					}
					aabb::Collider* BlockCollider = blk->owner().get_component_ptr<aabb::Collider>();
					if (BlockCollider == nullptr)
					{
						continue;
					}
					if (!counttablevoxel(blk, trav))
					{
						continue;
					}
					geointersect::boxRayCollision PotentialCollision = geointersect::intersection(BlockCollider->globalbox(), nray);
					if (PotentialCollision && PotentialCollision.unwrap().dist<ray_length && (!Collision || PotentialCollision.unwrap().dist<Collision.unwrap().dist()))
					{ 
						Collision = PotentialCollision.map([&](geointersect::RayHit hit){return RayWorldHit(hit,blk->owner().get_component<aabb::Collider>());});
					}
				 	
					
				}
			}
		}
		if (Collision)
		{
			return Collision;

		}
		float min_dist = std::numeric_limits<float>::infinity();
		size_t min_ind = 0;
		for (size_t i = 0; i < 3; i++)
		{
			if (sgns[i] == 0)
			{
				continue;
			}
			float new_val = abs(Boundry[i] - pos[i]) * conv_each[i];
			if (new_val < min_dist)
			{
				min_dist = new_val;
				min_ind = i;

			}
		}

		pos += norm_ray * min_dist;
		travel_dist += min_dist;
		curvox += Coord(sgns[min_ind], min_ind);
		Boundry += Coord(sgns[min_ind], min_ind);

	}



	return stn::None;
}
//todo remove this will not be needed as we will do a better function
//essentially goes to the next block and then goes backwords
block* voxtra::findprevblock(ray nray, GridTraverseMode trav)
{
	WorldRayCollision Intersection = travvox(nray, trav);
	if (!Intersection)
	{
		return nullptr;
	}
	float BackMag = 1 / 1000.f;
	Point3 BackProp = Intersection.unwrap().intersection() - nray.dir() * BackMag;
	return	(CtxName::ctx.Grid->getBlock(CtxName::ctx.Grid->getVoxel(BackProp)));
}
