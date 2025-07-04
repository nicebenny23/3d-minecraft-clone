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


	
	if (blk->owner.getcomponent<aabb::Collider>().effector )
	{
		return trav != countnormal;

	}
	return true;
}

bool voxtra::Boxcollwithgrid(geometry::Box Box )
{

		array<block*>& BlocksInRange= CtxName::ctx.Grid->voxelinrange(Box);

	for (block* PotentialCollision : BlocksInRange)
	{
		
				if (PotentialCollision == nullptr)
				{
					continue;
				}

				aabb::Collider* Collider = PotentialCollision->owner.getcomponentptr<aabb::Collider>();

			
				if (Collider!=nullptr&&PotentialCollision->attributes.solid&&!Collider->effector)
				{
					if (aabb::aabbboxintersect(Box, *Collider))
					{
						BlocksInRange.destroy();
						return true;
					}
				}


	
	}
	BlocksInRange.destroy();
	return false;




}

voxtra::WorldRayCollision  voxtra::travvox(ray nray, float acc, GridTraverseMode trav)
{
	float RayDistance = nray.length();
	v3::Vector3 Offset = nray.diff() / acc;
	v3::Vector3 Position = nray.start;
	 
	for (int i = 0; i < RayDistance * acc; i++)
	{

		Coord prevpos = CtxName::ctx.Grid->getVoxel(Position);
		Position += Offset;
		Coord curvox = CtxName::ctx.Grid->getVoxel(Position);
		if (curvox != prevpos)
		{

			block* blk = CtxName::ctx.Grid->getBlock(curvox);
			if (blk == nullptr)
			{
				continue;
			}
			aabb::Collider* BlockCollider = blk->owner.getcomponentptr<aabb::Collider>();
			if (BlockCollider == nullptr)
			{
				continue;
			}

					if (counttablevoxel(blk, trav))
					{
						geointersect::boxRayCollision Collision= geointersect::intersection(BlockCollider->globalbox(), nray);
						if (Collision)
						{
							return Opt::Construct<RayWorldHit>(Collision.unwrap(), &blk->owner.getcomponent<aabb::Collider>());
						}
					}
		}



	}
	return Opt::None;
}

//todo remove this will not be needed as we will do a better function
//essentially goes to the next block and then goes backwords
	block* voxtra::findprevblock(ray nray, float acc,GridTraverseMode trav)
	{
		WorldRayCollision Intersection=travvox(nray, acc, trav);
		if (!Intersection)
		{
			return nullptr;
		}
		float BackMag = 2 * nray.length() / acc;
		Vector3 BackProp= Intersection.unwrap().Intersection() - nray.dir() * BackMag;
	return	(CtxName::ctx.Grid->getBlock(CtxName::ctx.Grid->getVoxel( BackProp)));
	}
