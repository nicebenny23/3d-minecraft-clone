#include "collision.h"
#include "objecthelper.h"
#include "../world/managegrid.h"
#include "../game/rigidbody.h"
using namespace objutil;
void collision::update()
{
	for (int i = 0; i < Colliderlist.length; i++)
	{

		if (Colliderlist[i] != nullptr) {
			Vector3 off = Colliderlist[i]->collideroffset;
			Colliderlist[i]->center = off + objutil::toent(Colliderlist[i]->owner).transform.position;
		}
	}
	for (int i = 0; i < Colliderlist.length; i++)
	{
		if (Colliderlist[i] != nullptr) {
			handleCollisionWithGrid(*Colliderlist[i]);


		}
	}
	handleduelentitycollisions();
	for (int i = 0; i < Colliderlist.length; i++)
	{
		if (Colliderlist[i] != nullptr) {

			Colliderlist[i]->prevpos = Colliderlist[i]->center;
		}
	}
}

void collision::sendplayercameraray()
{

	ray cameraray = ray(Vector3(camera::campos), Vector3(camera::campos) + camera::direction() * 7);
	voxtra::RayCollisionWithGrid 	closest = collision::raycastall(cameraray);
	if (closest.box!=nullptr)
	{
		for (int i = 0; i < closest.box->owner->complist.length; i++) {
			closest.box->owner->complist[i]->onplayerclick();

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
	for (int i = 0; i < Colliderlist.length; i++)
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
	for (int i = 0; i < Colliderlist.length; i++)
	{
		if (Colliderlist[i] != nullptr) {


			if (Colliderlist[i]->owner == orgin)
			{
				continue;
			}


			Collider* coll = Colliderlist[i];
			if (distance(coll->center, nray.start) < nray.length())
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

voxtra::RayCollisionWithGrid collision::raycastCollisionWithGrid(ray nray) {

	voxtra::RayCollisionWithGrid closest = voxtra::RayCollisionWithGrid();
	closest.dist = INFINITY;
	for (int i = 0; i < totalgridsize; i++)
	{
		for (int j = 0; j < chunksize; j++) {
			block* blk = &grid::chunklist[i]->blockbuf[j];

			//for now gyrantee tha it has no aabb
			if (blk->solid) {
				if (distance(nray.start, blk->center()) < nray.length())
				{


					aabbraycolinfo blkinter = blk->getcomponent<Collider>().distanceonray(nray);
					if (blkinter.collided && blkinter.dist < closest.dist)
					{
						closest.colpoint = blkinter.intersectionpoint;
						closest.box = &(blk->getcomponent<Collider>());
						closest.dist = blkinter.dist;
					}
				}
			}

		}
	}
	return closest;



}
void componentcollisionsend(gameobject::obj* reciever, gameobject::obj* collided) {

	for (int i = 0; i < reciever->complist.length; i++)
	{
		reciever->complist[i]->oncollision(collided);
	}
}
void moveobj(v3::Vector3 force,entityname::entity* object) {
	array<Collider*> collist = object->getcomponents<Collider>();
	if (magnitude(force)>1000)
	{
		force /= 2;
		int l = 1;
	}
	for (int i = 0; i < collist.length; i++)
	{
		collist[i]->center += force;

		collist[i]->prevpos+= force;
	}
	object->transform.position += force;
}
void collision::handleduelentitycollisions()
{
	

	for (int i = 0; i < Colliderlist.length; i++)
	{
		
		if (Colliderlist[i] == nullptr) {
			continue;
		}
		for (int j = 0; j < Colliderlist.length; j++)
		{

			if ( Colliderlist[j] == nullptr) {

				continue;
			}

			if (Colliderlist[i]->owner==Colliderlist[j]->owner) {
				continue;
			}

			v3::Vector3 force = aabb::collideaabb(*Colliderlist[i], *Colliderlist[j]);
			if (v3::magnitude(force) > 0.00001f)
			{
				Vector3 actualforce = force / 2.0f;
				componentcollisionsend(Colliderlist[i]->owner, Colliderlist[j]->owner);
				componentcollisionsend(Colliderlist[j]->owner, Colliderlist[i]->owner);

				if (Colliderlist[i]->effector || Colliderlist[j]->effector) {

					continue;
				}
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

					moveobj(actualforce, &toent(Colliderlist[i]->owner));

				}
				if (!Colliderlist[j]->isunmovable)
				{
					moveobj(actualforce * -1, &toent(Colliderlist[j]->owner));
				}
			}
		}
	}
}
Vector3 getplaceoffset(Vector3 inter, Vector3 center, Vector3 Colliderscale) {

	Vector3 pos = zerov;
	pos.x = floorabs((inter.x - center.x) / Colliderscale.x);
	pos.y = floorabs((inter.y - center.y) / Colliderscale.y);
	pos.z = floorabs((inter.z - center.z) / Colliderscale.z);
	return pos;
}
bool collision::aabbCollidesWithEntity(Collider* blk, gameobject::obj* orgin ) {
	for (int i = 0; i < Colliderlist.length; i++)
	{
		if (Colliderlist[i] != nullptr)
		{

				if (Colliderlist[i]->owner == orgin)
				{
					continue;
				}
			
			if (aabbsintersect(*(Colliderlist[i]), *blk))
			{
				return true;
			}
		}
	}
	return false;
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
//todo implement movment per axis
void collision::handleCollisionWithGrid(Collider& entity)
{
	entity.center = toent(entity.owner).transform.position+entity.collideroffset;
	v3::Vector3 lowpos = entity.center - entity.scale - unitv;

	v3::Coord lowest = v3::Coord(floorabs(lowpos.x), floorabs(lowpos.y), floorabs(lowpos.z));
	v3::Vector3 highpos = entity.center + entity.scale + unitv;

	v3::Coord highest = v3::Coord(ceilabs(highpos.x), ceilabs(highpos.y), ceilabs(highpos.z));
	std::swap(entity.center, entity.prevpos);
	for (int i = 0; i < 3; i++)
	{
		
		switch (i)
		{

		case 0:
			entity.center.y = entity.prevpos.y;
			break;
		case 1:
			entity.center.z = entity.prevpos.z;
			break;
		case 2:
			entity.center.x = entity.prevpos.x;
			break;
		}
		
		Vector3 minforce = zerov;

		for (int x = lowest.x; x < highest.x; x++)
		{
			for (int y = lowest.y; y < highest.y; y++)
			{
				for (int z = lowest.z; z < highest.z; z++)
				{
					//getting solid at grid is optimization

					Vector3 force = colideentandblock(entity, getobjatgrid(x, y, z));
					
					if (magnitude(force) > magnitude(minforce) || magnitude(minforce) == 0)
					{

						minforce = force;
					}
				}
			}
		}


		if (!entity.isunmovable)
		{

			moveobj(minforce, &toent(entity.owner));
		}
		
	}


}


