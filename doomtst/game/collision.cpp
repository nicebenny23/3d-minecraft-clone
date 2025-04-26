#include "collision.h"
#include "objecthelper.h"
#include "../world/managegrid.h"
#include "../game/rigidbody.h"
using namespace objutil;
void collision::update()
{
	for (int iters = 0; iters < collisioniterations; iters++)
	{


		for (int i = 0; i < Colliderlist.size; i++)
		{
			if (Colliderlist[i] != nullptr) {
				handleCollisionWithGrid(*Colliderlist[i]);


			}
		}
		handleduelentitycollisions();
	}
}


bool collision::boxCollidesWithEntity(geometry::Box blk, gameobject::obj* orgin )
{
	for (int i = 0; i < Colliderlist.size; i++)
	{
		if (Colliderlist[i] != nullptr)
		{
		
				if (Colliderlist[i]->owner == orgin)
				{
					continue;
				}
			
			if (aabbboxintersect(blk,*Colliderlist[i]))
			{
				return true;
			}
		}
	}
	
	return false;
}

voxtra::RayWorldIntersection collision::raycastentity(ray nray, gameobject::obj* orgin )
{

	voxtra::RayWorldIntersection closest = voxtra::RayWorldIntersection();
	closest.dist = INFINITY;
	//
	for (int i = 0; i < Colliderlist.size; i++)
	{
		if (Colliderlist[i] != nullptr) {


			if (Colliderlist[i]->owner == orgin)
			{
				continue;
			}


			Collider* coll = Colliderlist[i];
			if (dist(coll->globalbox().center, nray.start) < nray.length())
			{

				geointersect::boxraycollision blkinter = geointersect::intersection( coll->globalbox(), nray);
				if (blkinter.collided && blkinter.dist < closest.dist)
				{
					closest.Ray = nray;
					closest.colpoint = blkinter.intersectionpoint;
					closest.collider = coll;
					closest.dist = blkinter.dist;
				}
			}
		}
	}
	return closest;
}

void propagatecollisionmessage(gameobject::obj* o1, gameobject::obj* o2) {

	for (int i = 0; i < o1->componentlist.length; i++)
	{
		o1->componentlist[i]->oncollision(o2);
	}
	for (int j= 0; j < o2->componentlist.length; j++)
	{
		o2->componentlist[j]->oncollision(o1);
	}
}
void moveobj(v3::Vector3 force,gameobject::obj* object) {
	
	if (object->type==gameobject::block)
	{
		Assert("block rigidbodies not supported yet");
	}
	else
	{

		toent(object).transform.position += force;
		

	}
}
void distributeforce( gameobject::obj* p1, gameobject::obj* p2,Vector3 force) {

	float totalforcemag = 0;
	float e1mass = 0;
	float e2mass = 0;
	if (p1->hascomponent<rigidbody>())
	{
		e1mass = p1->getcomponent<rigidbody>().mass;


	}

	if (p2->hascomponent<rigidbody>())
	{
		e1mass = p2->getcomponent<rigidbody>().mass;


	}


	totalforcemag = e1mass + e2mass;


	if (totalforcemag == 0)
	{
		return;
	}
	if (e2mass == 0)
	{
		moveobj(force, p1);
		return;
	}
	if (e1mass == 0)
	{
		moveobj(force * -1, p2);
		return;
	}

	moveobj(force * e2mass / totalforcemag, p1);

	moveobj(force * e1mass / totalforcemag * -1, p2);


}
void collision::handleduelentitycollisions()
{
	

	for (int i = 0; i < Colliderlist.size; i++)
	{
		
		if (Colliderlist[i] == nullptr) {
			continue;
		}
		for (int j = 0; j < Colliderlist.size; j++)
		{

			if ( Colliderlist[j] == nullptr) {

				continue;
			}

			if (Colliderlist[i]->owner==Colliderlist[j]->owner) {
				continue;
			}

			v3::Vector3 force = aabb::collideaabb(*Colliderlist[i], *Colliderlist[j]);
			if (v3::mag(force) > 0.01f)
			{
			
				propagatecollisionmessage(Colliderlist[i]->owner, Colliderlist[j]->owner);
				
				if (Colliderlist[i]->effector || Colliderlist[j]->effector) {

					continue;
				}
				
				distributeforce(Colliderlist[i]->owner, Colliderlist[j]->owner, force);
				
			}
		}
	}
}
bool collision::aabbCollidesWithEntity(Collider* blk, gameobject::obj* orgin ) {
	return collision::boxCollidesWithEntity(blk->globalbox(), orgin);
}

voxtra::RayWorldIntersection collision::raycastall(ray nray, gameobject::obj* orgin, voxtra::GridTraverseMode travmode)
{
	voxtra::RayWorldIntersection gridcol = voxtra::travvox(nray,200,travmode);
	voxtra::RayWorldIntersection entcol = raycastentity(nray,orgin);
	if (gridcol.dist<entcol.dist)
	{
		return gridcol;
	}
	return entcol;
}
Vector3 colideentandblock(Collider& entity, block* tocollide) {
	
	if (tocollide != nullptr)
	{
		if (tocollide->hascomponent<aabb::Collider>())
		{


			aabb::Collider& blockcol = tocollide->getcomponent<aabb::Collider>();



			Vector3 my = entity.globalbox().center;
			Vector3 otherpos = blockcol.globalbox().center;
			Vector3 force = aabb::collideaabb(entity, blockcol);


			if (force == zerov)
			{
				return zerov;
			}
			//bug collision was happening twice(fix,improve collision to not collide already destroyed/being destroyed obejcts)/
			propagatecollisionmessage(entity.owner, tocollide);
			

			if (!blockcol.effector && !entity.effector)
			{

				return force;
			}


		}
	}
	return zerov;
}

void collision::handleCollisionWithGrid(Collider& entity)
{
	array<block*>& blklist = CtxName::ctx.Grid->voxelinrange(entity.globalbox());

	//std::swap(entity.box.center, entity.prevpos);
	
		
	
		Vector3 minforce = zerov;
		block* minblock=nullptr;
		for (int ind = 0; ind < blklist.length; ind++)
		{


	
			Vector3 force = colideentandblock(entity, blklist[ind]);

			if (mag(force) > mag(minforce) || mag(minforce) == 0)
			{
				minblock = blklist[ind];
				minforce = force;
			}
		}
		if (minblock!=nullptr)
		{
			if (!entity.effector)
			{

				distributeforce(entity.owner, minblock, minforce);

			}
		}
	
	blklist.destroy();

	delete& blklist;
		
}





