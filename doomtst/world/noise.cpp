#pragma once
#include "noise.h"
using namespace stn;
#include "../util/mathutil.h"

//todo move this to random

static const int PrimeX = 501125321;
static const int PrimeY = 1136930381;
static const int PrimeZ = 1720413743;

noisemap::noisemap()
{

}


float noisemap::Eval(Coord pos)
{

	return Eval(Vec3(pos));
}
float noisemap::Eval(Vec3 pos)
{

	return equalizer.apply_distribution((EvaluateNoiseAtPoint(pos+ NoiseOffset, properties)));
}


inline float dotGridGradient( const int& x, const int& y, const int& z, const Vec3& pos) {

	Vec3 gradient = randompointonsphere(x,y,z);
	
	float dx = pos.x - x;
	float dy = pos.y - y;
	float dz = pos.z - z;
	float rval= (dx * gradient.x + dy * gradient.y + dz * gradient.z);
	return rval;
	
}

inline float dotGridGradient( int x, int y, int z, float xd, float yd, float zd)
{
	Vec3 pos = randompointonsphere(x, y, z);
	return xd * pos.x+ yd * pos.y+ zd * pos.z;
}

float interpolatenoisemap(float x, float y, float z)
{
	int x0 = FastFloor(x);
	int y0 = FastFloor(y);
	int z0 = FastFloor(z);

	float xd0 = (float)(x - x0);
	float yd0 = (float)(y - y0);
	float zd0 = (float)(z - z0);
	float xd1 = xd0 - 1;
	float yd1 = yd0 - 1;
	float zd1 = zd0 - 1;

	float xs = interoplatequintic(xd0);
	float ys = interoplatequintic(yd0);
	float zs = interoplatequintic(zd0);

	x0 *= PrimeX;
	y0 *= PrimeY;
	z0 *= PrimeZ;
	int x1 = x0 + PrimeX;
	int y1 = y0 + PrimeY;
	int z1 = z0 + PrimeZ;
	float n0, n1, ix0, ix1, value1, value2;
	n0 = dotGridGradient(x0, y0, z0, xd0,yd0,zd0);

	n1 = dotGridGradient(x1, y0, z0, xd1, yd0, zd0);
	ix0 = lerp(n0, n1, xs);

	n0 = dotGridGradient(x0, y1, z0, xd0, yd1, zd0);
	n1 = dotGridGradient(x1, y1, z0,  xd1, yd1, zd0);
	ix1 = lerp(n0, n1, xs);

	value1 = lerp(ix0, ix1, ys);
	n0 = dotGridGradient(x0, y0, z1, xd0, yd0, zd1);
	n1 = dotGridGradient(x1, y0, z1, xd1, yd0, zd1);
	ix0 = lerp(n0, n1, xs);

	n0 = dotGridGradient(x0, y1, z1, xd0, yd1, zd1);
	n1 = dotGridGradient(x1, y1, z1, xd1, yd1, zd1);
	ix1 = lerp(n0, n1, xs);

	value2 = lerp(ix0, ix1, ys);
 return lerp(value1, value2, zs);

	
}

float EvaluateNoiseAtPoint(Vec3 point,noiseparams params)
{
	float value = 0;
	float maxintensity = 0;
	float intensity = 1;

	float scale = params.startscale;
	for (int i = 0; i < params.octaves; i++)
	{
		Vec3 testpoint = point * scale;
		float toadd = intensity * interpolatenoisemap(testpoint.x, testpoint.y, testpoint.z);
		value += toadd;
		maxintensity += intensity;

		scale *= params.scalefactor;
		intensity *= params.amplificationfactor;

	}
	return value / maxintensity;
}
void noisemap::create()
{	
	createdist();
	
}

void noisemap::destroy()
{

	equalizer.destroy();
	
	
}

