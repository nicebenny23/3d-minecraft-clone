#include "collision.h"
#include "objecthelper.h"
#include "../world/managegrid.h"
#include "../game/rigidbody.h"
using namespace objutil;
void collision::update()
{
	for (int i = 0; i < Colliderlist.size; i++)
	{

		if (Colliderlist[i] != nullptr) {
			Vector3 off = Colliderlist[i]->collideroffset;
			Colliderlist[i]->box.center = off + objutil::toent(Colliderlist[i]->owner).transform.position;
		}
	}
	for (int i = 0; i < Colliderlist.size; i++)
	{
		if (Colliderlist[i] != nullptr) {
			handleCollisionWithGrid(*Colliderlist[i]);


		}
	}
	handleduelentitycollisions();
	for (int i = 0; i < Colliderlist.size; i++)
	{
		if (Colliderlist[i] != nullptr) {

			Colliderlist[i]->prevpos = Colliderlist[i]->box.center;
		}
	}
}

void collision::sendplayercameraray()
{

	ray cameraray = ray(Vector3(camera::campos), Vector3(camera::campos) + camera::direction() * 7);
	voxtra::RayCollisionWithGrid 	closest = collision::raycastall(cameraray);
	if (closest.box!=nullptr)
	{
		for (int i = 0; i < closest.box->owner->componentlist.length; i++) {
			closest.box->owner->componentlist[i]->onplayerclick();

		}
	}
}

bool collision::boxCollidesWithEntity(geometry::Box blk, gameobject::obj* orgin )
{
	bool orginwelldefined = false;
	if (orgin!=nullptr)
	{
		orginwelldefined = true;
	}
	for (int i = 0; i < Colliderlist.size; i++)
	{
		if (Colliderlist[i] != nullptr)
		{
			if (orginwelldefined)
			{
				if (Colliderlist[i]->owner == orgin)
				{
					continue;
				}
			}
			if (aabbboxintersect(blk,*Colliderlist[i]))
			{
				return true;
			}
		}
	}
	
	return false;
}

voxtra::RayCollisionWithGrid collision::raycastentity(ray nray, gameobject::obj* orgin )
{
	bool orginwelldefined = false;
	if (orgin != nullptr)
	{
		orginwelldefined = true;
	}
	voxtra::RayCollisionWithGrid closest = voxtra::RayCollisionWithGrid();
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
			if (distance(coll->box.center, nray.start) < nray.length())
			{

				aabbraycolinfo blkinter = coll->distanceonray(nray);
				if (blkinter.collided && blkinter.dist < closest.dist)
				{
					closest.colpoint = blkinter.intersectionpoint;
					closest.box = coll;
					closest.dist = blkinter.dist;
				}
			}
		}
	}
	return closest;
}

void componentcollisionsend(gameobject::obj* reciever, gameobject::obj* collided) {

	for (int i = 0; i < reciever->componentlist.length; i++)
	{
		reciever->componentlist[i]->oncollision(collided);
	}
}
void moveobj(v3::Vector3 force,entityname::entity* object) {
	array<Collider*>& collist = object->getcomponents<Collider>();
	if (magnitude(force)>1000)
	{
		force /= 2;
	}
	for (int i = 0; i < collist.length; i++)
	{
		collist[i]->box.center += force;

		collist[i]->prevpos+= force;
	}
	object->transform.position += force;
	collist.destroy();
	delete& collist;
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
			if (v3::magnitude(force) > 0.01f)
			{
			
				componentcollisionsend(Colliderlist[i]->owner, Colliderlist[j]->owner);
				componentcollisionsend(Colliderlist[j]->owner, Colliderlist[i]->owner);

				if (Colliderlist[i]->effector || Colliderlist[j]->effector) {

					continue;
				}
				force /= 2;
				if (Colliderlist[i]->isunmovable)
				{
					force *= 2;

				}
				if (Colliderlist[j]->isunmovable)
				{
					force *= 2;

				}
				
				if (!Colliderlist[i]->isunmovable)
				{

					moveobj(force, &toent(Colliderlist[i]->owner));

				}
				if (!Colliderlist[j]->isunmovable)
				{
					moveobj(force* -1, &toent(Colliderlist[j]->owner));
				}
			}
		}
	}
}
bool collision::aabbCollidesWithEntity(Collider* blk, gameobject::obj* orgin ) {
	return collision::boxCollidesWithEntity(blk->box, orgin);
}

voxtra::RayCollisionWithGrid collision::raycastall(ray nray, gameobject::obj* orgin, voxtra::gridtrav travmode)
{
	voxtra::RayCollisionWithGrid gridcol = voxtra::travvox(nray,200,travmode);
	voxtra::RayCollisionWithGrid entcol = raycastentity(nray,orgin);
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



			Vector3 force = aabb::collideaabb(entity, blockcol);


			if (force == zerov)
			{
				return zerov;
			}


			componentcollisionsend(entity.owner, tocollide);
			componentcollisionsend(tocollide, entity.owner);

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
	array<block*>& blklist = grid::voxelinrange(geometry::Box(entity.box.center, entity.box.scale));

	std::swap(entity.box.center, entity.prevpos);
	for (int i = 0; i < 3; i++)
	{
		
		switch (i)
		{

		case 0:
			entity.box.center.y = entity.prevpos.y;
			break;
		case 1:
			entity.box.center.z = entity.prevpos.z;
			break;
		case 2:
			entity.box.center.x = entity.prevpos.x;
			break;
		}

		Vector3 minforce = zerov;

		for (int ind = 0; ind < blklist.length; ind++)
		{


			//getting solid at grid is optimization

			Vector3 force = colideentandblock(entity, blklist[ind]);

			if (magnitude(force) > magnitude(minforce) || magnitude(minforce) == 0)
			{

				minforce = force;
			}
		}
		if (!entity.isunmovable)
		{

			moveobj(minforce, &toent(entity.owner));
		}
	}
	blklist.destroy();

	delete& blklist;
		
}





