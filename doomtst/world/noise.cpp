#include "noise.h"
using namespace dynamicarray;
array<v3::Vector3> seededdirections;
noisemap::noisemap()
{

}


float noisemap::operator[](Coord pos)
{

	return applydist(computenoiseatpoint(pos+Vector3(8.31553,8.7191,3.40505), properties));
}
float noisemap::operator[](Vector3 pos)
{

	return applydist(computenoiseatpoint(pos+ Vector3(7.3113f, 8.74191f, 3.40505f), properties));
}


inline float dotGridGradient( const int& x, const int& y, const int& z, const Vector3& pos) {

	Vector3 gradient = randompointonsphere(x,y,z);
	
	float dx = pos.x - x;
	float dy = pos.y - y;
	float dz = pos.z - z;
	float rval= (dx * gradient.x + dy * gradient.y + dz * gradient.z);
	return rval;
	
}

static const int PrimeX = 501125321;
static const int PrimeY = 1136930381;
static const int PrimeZ = 1720413743;
static int Hash(int seed, int xPrimed, int yPrimed, int zPrimed)
{
	int hash = seed ^ xPrimed ^ yPrimed ^ zPrimed;

	hash *= 0x27d4eb2d;
	return hash;
}
inline Vector3 randompointonsphere(int x, int y, int z) {
	int seed = 3;
	int hash = Hash(seed, x, y, z);
	hash ^= hash >> 15;
	hash &= MAXSHORT - 1;
	return seededdirections.fastat(hash);
	
}
inline float dotGridGradient( int x, int y, int z, float xd, float yd, float zd)
{
	Vector3 pos = randompointonsphere(x, y, z);
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
	ix0 = interpolate(n0, n1, xs);

	n0 = dotGridGradient(x0, y1, z0, xd0, yd1, zd0);
	n1 = dotGridGradient(x1, y1, z0,  xd1, yd1, zd0);
	ix1 = interpolate(n0, n1, xs);

	value1 = interpolate(ix0, ix1, ys);
	n0 = dotGridGradient(x0, y0, z1, xd0, yd0, zd1);
	n1 = dotGridGradient(x1, y0, z1, xd1, yd0, zd1);
	ix0 = interpolate(n0, n1, xs);

	n0 = dotGridGradient(x0, y1, z1, xd0, yd1, zd1);
	n1 = dotGridGradient(x1, y1, z1, xd1, yd1, zd1);
	ix1 = interpolate(n0, n1, xs);

	value2 = interpolate(ix0, ix1, ys);
 return interpolate(value1, value2, zs);

	
}

float computenoiseatpoint(Vector3 point,noiseparams params)
{
	float value = 0;
	float maxintensity = 0;
	float intensity = 1;

	float scale = params.startscale;
	for (int i = 0; i < params.octaves; i++)
	{
		Vector3 testpoint = point * scale;
		float toadd = intensity * interpolatenoisemap(testpoint.x, testpoint.y, testpoint.z);
		if (params.type==normalnoise)
		{

			value += toadd;

		}
		if (params.type == rigid)
		{

			value += abs(toadd);
		}
		maxintensity += intensity;

		scale *= params.scalefactor;
		intensity *= params.amplificationfactor;

	}
	return value / maxintensity;
}
int comparefloat(const void* b, const void* a)
{
	return sign(*((float*)a) - *((float*)b));
}
void noisemap::create()
{	
	createdist();
	
}

void noisemap::destroy()
{

	distribution.destroy();
	
	
}

inline float noisemap::applydist(const float val)
{

return distribution.fastat(getbucket(val));

}

void initrandomdirs()
{
	const int startingseed = 5;
	
	unsigned int noiseval = startingseed;
	for (int i = 0; i < 100; i++)
	{
		randomcoord(noiseval);
	}
	float nabs = 0;
	seededdirections = array<Vector3>(USHRT_MAX,false);
	for (int i = 0; i < USHRT_MAX; i++)
	{
		
		Vector3 gradatind;
		do
		{

			randomcoord(noiseval);
			randomcoord(noiseval);
			randomcoord(noiseval);
			gradatind.x = noiseval;
			randomcoord(noiseval);
			randomcoord(noiseval);
			randomcoord(noiseval);
			gradatind.y = noiseval;
			randomcoord(noiseval);
			randomcoord(noiseval);
			randomcoord(noiseval);
			gradatind.z = noiseval;
			gradatind /= static_cast<float>(MAXUINT32);
			gradatind -= unitv / 2;
			gradatind * 2;
		} while (magnitude2(gradatind)>1);
		seededdirections[i] = normal(gradatind);
	}

}
