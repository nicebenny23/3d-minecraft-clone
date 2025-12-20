
#include "collision.h"
#include "objecthelper.h"
#include "../world/managegrid.h"
#include "../game/rigidbody.h"
#include "query.h"
void collision::update()
{
	for (int iters = 0; iters < collisioniterations; iters++)
	{
		for (Collider* Coll : Colliderlist)
		{
			handleCollisionWithGrid(*Coll, (iters == 0));
		}
	}

	handleduelentitycollisions();
}


bool collision::boxCollidesWithEntity(geometry::Box blk, HitQuery query)
{

	for (Collider* Collider1 : Colliderlist)
	{



		if (Collider1->owner() == query.orgin)
		{
			continue;
		}

		if (aabbboxintersect(blk, *Collider1))
		{
			return true;
		}

	}

	return false;
}

voxtra::WorldRayCollision collision::raycastentity(ray nray, HitQuery query)
{
	voxtra::WorldRayCollision closest = stn::None;
	for (Collider* Collider1 : Colliderlist)
	{
		if (query.orgin && Collider1->owner() == query.orgin.unwrap())
		{
			continue;
		}
		if (dist(Collider1->globalbox().center, nray.start) < nray.length())
		{

			geointersect::boxRayCollision blkinter = geointersect::intersection(Collider1->globalbox(), nray);
			if (blkinter && (closest == stn::None || blkinter.unwrap().dist < closest.unwrap().dist()))
			{
				closest = voxtra::RayWorldHit(blkinter.unwrap(), *Collider1);
			}
		}

	}
	return closest;
}
struct collision_event {
	template<ecs::ComponentType T>
	bool source_has_component() const{
		return source.has_component<T>();
	}
	template<ecs::ComponentType T>
	T& source_get_component() const{
		return source.get_component<T>();
	}
	template<ecs::ComponentType T>
	bool target_has_component() {
		return target.has_component<T>();
	}
	template<ecs::ComponentType T>
	T& target_get_component() {
		return target.get_component<T>();
	}
	//the target and source are abstrctions for dealing with events and for now each event wil lalso have a 
	ecs::obj target;
	ecs::obj source;

	collision_event(ecs::obj o1, ecs::obj o2) :source(o1), target(o2) {
	}
};
void propagatecollisionmessage(ecs::obj o1, ecs::obj o2) {
	o1.world()->write_event<collision_event>(o1, o2);
	o2.world()->write_event<collision_event>(o2, o1);
}
void moveobj(v3::Vec3 force, ecs::obj object) {

	if (object.has_component<blockname::block>())
	{
		Assert("block rigidbodies not supported yet");
	}
	else
	{
		object.get_component<ecs::transform_comp>().transform.position += force;
	}
}Vec3 colideentandblock(Collider& entity, block* tocollide, bool is_trigger) {
	if (tocollide == nullptr || !tocollide->owner().has_component<aabb::Collider>())
	{

		return zerov;
	}
	aabb::Collider& blockcol = tocollide->owner().get_component<aabb::Collider>();
	Point3 my = entity.globalbox().center;
	Point3 otherpos = blockcol.globalbox().center;
	Option<Vec3> force = aabb::collideaabb(entity, blockcol);
	if (!force)
	{
		return zerov;
	}
	if (is_trigger)
	{
		propagatecollisionmessage(entity.owner(), tocollide->owner());
	}
	if (!blockcol.effector && !entity.effector)
	{
		return force.unwrap();
	}
	return zerov;
	
}

void distributeforce(ecs::obj p1, ecs::obj p2, Vec3 force) {

	colideentandblock(p1.get_component<Collider>(), p2.get_component_ptr<block>(),false);
	float totalforcemag = 0;
	float e1mass = 0;
	float e2mass = 0;
	if (p1.has_component<rigidbody>())
	{
		e1mass = p1.get_component<rigidbody>().mass;
	}
	if (p2.has_component<rigidbody>())
	{
		e1mass = p2.get_component<rigidbody>().mass;
	}
	totalforcemag = e1mass + e2mass;
	if (totalforcemag == 0)
	{
		return;
	}
	if (e2mass == 0)
	{
		moveobj(force*-1.0, p1);
		moveobj(force, p1);

		colideentandblock(p1.get_component<Collider>(), p2.get_component_ptr<block>(), false);
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
	for (Collider* Collider1 : Colliderlist)
	{
		for (Collider* Collider2 : Colliderlist)
		{
			if (Collider1->owner() == Collider2->owner()) {

				continue;
			}
			Option<v3::Vec3> force = aabb::collideaabb(*Collider1, *Collider2);
			if (force.is_none())
			{
				continue;
			}
			propagatecollisionmessage(Collider1->owner(), Collider2->owner());

			if (Collider1->effector || Collider2->effector) {

				continue;
			}
			distributeforce(Collider1->owner(), Collider2->owner(), force.unwrap());
		}
	}
}
bool collision::aabbCollidesWithEntity(Collider* blk, HitQuery query) {
	return collision::boxCollidesWithEntity(blk->globalbox(), query);
}
voxtra::WorldRayCollision collision::raycastall(ray nray, HitQuery query, voxtra::GridTraverseMode travmode)
{
	voxtra::WorldRayCollision gridcol = voxtra::travvox(nray, travmode);
	voxtra::WorldRayCollision entcol = raycastentity(nray, query);
	if (gridcol == stn::None) {
		return entcol;
	}
	if (entcol == stn::None)
	{
		return gridcol;
	}
	if (gridcol.unwrap().dist() < entcol.unwrap().dist())
	{
		return gridcol;
	}
	return entcol;
}

void collision::handleCollisionWithGrid(Collider& entity, bool is_trigger)
{
	geometry::Box entity_box = entity.globalbox();
	array<block*> blklist = CtxName::ctx.Grid->voxelinrange(entity_box);
	Vec3 minforce = zerov;
	block* minblock = nullptr;

	for (int ind = 0; ind < blklist.length(); ind++)
	{

		Vec3 force = colideentandblock(entity, blklist[ind], is_trigger);
		if (force.length() > minforce.length() || minforce.length() == 0)
		{
			minblock = blklist[ind];
			minforce = force;
		}
	}
	if (minblock != nullptr)
	{
		if (!entity.effector)
		{

			distributeforce(entity.owner(), minblock->owner(), minforce);

		}
	}
}