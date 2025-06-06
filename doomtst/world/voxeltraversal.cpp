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


	
	if (blk->getcomponent<aabb::Collider>().effector )
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

				aabb::Collider* Collider = PotentialCollision->getcomponentptr<aabb::Collider>();

			
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

voxtra::RayWorldIntersection  voxtra::travvox(ray nray, float acc, GridTraverseMode trav)
{
	float RayDistance = nray.length();
	v3::Vector3 Offset = nray.diff() / acc;
	v3::Vector3 Position = nray.start;
	 
	for (int i = 0; i < RayDistance * acc; i++)
	{

		Coord prevpos = CtxName::ctx.Grid->getvoxellocation(Position);
		Position += Offset;
		Coord curvox = CtxName::ctx.Grid->getvoxellocation(Position);
		if (curvox != prevpos)
		{

			block* blk = CtxName::ctx.Grid->getBlock(curvox);
			if (blk == nullptr)
			{
				continue;
			}
			aabb::Collider* BlockCollider = blk->getcomponentptr<aabb::Collider>();
			if (BlockCollider == nullptr)
			{
				continue;
			}

					if (counttablevoxel(blk, trav))
					{
						geointersect::boxraycollision Collision= geointersect::intersection(BlockCollider->globalbox(), nray);
						if (Collision.collided)
						{


							return voxtra::RayWorldIntersection(Collision.dist, &blk->getcomponent<aabb::Collider>(), Collision.intersectionpoint, nray);
						}
					}
		}



	}
	return voxtra::RayWorldIntersection();
}

//todo remove this will not be needed as we will do a better function
//essentially goes to the next block and then goes backwords
	block* voxtra::findprevblock(ray nray, float acc,GridTraverseMode trav)
	{
		RayWorldIntersection Intersection=travvox(nray, acc, trav);
		if (!Intersection.intersected())
		{
			return nullptr;
		}
		float BackMag = 2 * nray.length() / acc;
		Vector3 BackProp= Intersection.colpoint- nray.dir()*BackMag;
	return	(CtxName::ctx.Grid->getBlock(CtxName::ctx.Grid->getvoxellocation( BackProp)));
	}
