#include "gameobject.h"
#include "../world/voxeltraversal.h"
#ifndef navmesh_HPP
#define navmesh_HPP

struct navagentpoint
{
	v3::Vector3 location;
	navagentpoint(v3::Vector3 loc,float parentdist);
	float dist;
	float disptancetoend();
	float distance(v3::Vector3 start);
};
struct navparent;
struct navfollow:gameobject::component
{
	
	navparent* following;
	v3::Vector3 followingpos;
	void update();
	navparent* parent;
	
	navfollow(navparent* parentagent);
	float timetillupdate;
	void getcurrentpath();
	bool  seesparent();
	bool seespoint(v3::Vector3 point);
	float distancethroughpoint(navagentpoint point);
};
struct navparent:gameobject::component {
	float navgridoffset;
	int navamt;
	float timetillupdate;
	void update();
	array<navagentpoint> navpoints;
	void generatenavagentpoints();
	float dist(v3::Vector3 point);
};


#endif // !
