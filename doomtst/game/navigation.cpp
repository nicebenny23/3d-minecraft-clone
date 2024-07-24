#include "navigation.h"
#include "objecthelper.h"
#include "../util/time.h"
navagentpoint::navagentpoint(v3::Vector3 loc,float parentdist)
{
	location = loc;
	dist = parentdist;
}
float navagentpoint::distance(v3::Vector3 start)
{
    return 0.0f;
}



bool navfollow::seespoint(v3::Vector3 point)
{
	ray raytoplayer = ray(objutil::toent(owner).transform.position, point);
	if (voxtra::raycolllideswithgrid(raytoplayer, 10, false))
	{
		return true;
	}
	return false; 
}
bool navfollow::seesparent()
{
	return seespoint( objutil::toent(parent->owner).transform.position);
	
}

void navfollow::update()
{
	timetillupdate -= timename::dt;
	if (timetillupdate < 0)
	{
		timetillupdate = 1;
		getcurrentpath();
	}
}

navfollow::navfollow(navparent* parentagent)
{
	parent = parentagent;
}

void navfollow::getcurrentpath()
{
	v3::Vector3 gotopoint=zerov;
		if (seesparent()) {
				v3::Vector3 ownerpoint = objutil::toent(parent->owner).transform.position;

			gotopoint = ownerpoint;
			
	    }
		else
		{
			float mindist = 100000;
			for (int i = 0; i < parent->navpoints.length; i++)
			{
				navagentpoint testpoint = parent->navpoints[i];
				float testdist= distancethroughpoint(testpoint);
				if (testdist<mindist)
				{
					gotopoint = testpoint.location;
				}
			}
		}
		followingpos = gotopoint;
}
//assumesp oint can be went through
float navfollow::distancethroughpoint(navagentpoint point) {
	if (seespoint(point.location))
	{
		v3::Vector3 thispoint = objutil::toent(owner).transform.position;
		v3::Vector3 ownerpoint = objutil::toent(parent->owner).transform.position;
		return distance(point.location,thispoint) + distance(thispoint, ownerpoint);
	}
	return 100000;
}
void navparent::update()
{
	timetillupdate -= timename::dt;
	if (timetillupdate<0)
	{
		timetillupdate = 1;
		generatenavagentpoints();
	}
}
void navparent::generatenavagentpoints()
{
	navpoints.destroy();
	navpoints = array<navagentpoint>();
	v3::Vector3 pos = objutil::toent(owner).transform.position;
	for (int x= -navamt; x<navamt; x++)
	{
		for (int y = -navamt; y < navamt; y++)
		{
			for (int z= -navamt; z < navamt; z++)
			{
				v3::Vector3 offset = pos +v3::Vector3(x, y, z) * navgridoffset;
				float navmeshdist = dist(offset);
				if (navmeshdist!=-1)
				{
					navpoints.append(navagentpoint(offset,navmeshdist));
				}
			}
		}
	}
}

float navparent::dist(v3::Vector3 point)
{
	v3::Vector3 pos = objutil::toent(owner).transform.position;
	ray raytopoint = ray(pos, point);
	if (voxtra::raycolllideswithgrid(raytopoint,10,false))
	{
		return raytopoint.length();
	}
	return -1;
}
