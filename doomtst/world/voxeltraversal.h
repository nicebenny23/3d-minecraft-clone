#include "grid.h"
#include "../util/ray.h"
#include "../game/aabb.h"
#include "../util/geometry.h"
#ifndef voxtrav_Hpp
#define voxtrav_Hpp
namespace voxtra {
	inline Coord   getcurrvoxel(Vector3 pos) {
		return  Coord(std::floor(pos.x), std::floor(pos.y), std::floor(pos.z));
	}
	struct RayCollisionWithGrid
	{
		aabb::Collider* box;
		v3::Vector3 colpoint;
		float dist;

		RayCollisionWithGrid(float distance, aabb::Collider* closestbox, v3::Vector3 intpoint) {

			dist = distance;
			box = closestbox;
			colpoint = intpoint;
		}
		RayCollisionWithGrid() {
			box = nullptr;
			dist = 1;
			colpoint = zerov;
		}
	};
	bool raycolllideswithgrid(ray nray, float acc, bool counteffectors = false);
	bool Boxcollwithgrid(geometry::Box bx, float acc, bool counteffectors=false);
	RayCollisionWithGrid travvox(ray nray, float acc,bool counteffectors=false);
	block* findprevblock(ray nray, float acc);
}
#endif