#include "noise.h"
using namespace dynamicarray;
chunknoisemap::chunknoisemap(v3::Coord location)
{

	loc = location;
	noisemap = dynamicarray::array<float>(16 * 16 * 16);
	for (int i = 0; i < 16 * 16 * 16; i++)
	{
		noisemap[i] = 0;
	}
	maxint = 0;
}

Vector3 radat(Coord pos)
{
	return randompointonsphere(pos);
}

float chunknoisemap::operator[](int ind)
{
	return noisemap[ind] / maxint;
}

float chunknoisemap::operator[](Coord pos)
{

	return noisemap[pos.z + 16 * pos.y + 256 * pos.x] / maxint;
}
float& chunknoisemap::operator[](Vector3 pos)
{

	return noisemap[pos.z + 16 * pos.y + 256 * pos.x];
}


float dotGridGradient( Coord chunkpos, Vector3 pos) {

	Vector3 gradient = radat(chunkpos);


	float dx = pos.x - chunkpos.x;
	float dy = pos.y - chunkpos.y;
	float dz = pos.z - chunkpos.z;
	return (dx * gradient.x + dy * gradient.y + dz * gradient.z);
}


float interpolatenoisemap(float x, float y, float z,float scl) {

	x *=scl;
	y *= scl;
	z *= scl;
	int x0 = floor(x);
	int x1 = x0 + 1;
	int y0 = floor(y);
	int y1 = y0 + 1;
	int z0 = floor(z);
	int z1 = z0 + 1;

	Vector3 pos(x, y, z);
	float sx = x - x0;
	float sy = y - y0;
	float sz = z - z0;
	float n0, n1, ix0, ix1, value1, value2;
	n0 = dotGridGradient( Vector3(x0, y0, z0), pos);
	n1 = dotGridGradient(Vector3(x1, y0, z0), pos);
	ix0 = interpolate(n0, n1, sx);

	n0 = dotGridGradient( Vector3(x0, y1, z0), pos);
	n1 = dotGridGradient( Vector3(x1, y1, z0), pos);
	ix1 = interpolate(n0, n1, sx);

	value1 = interpolate(ix0, ix1, sy);
	n0 = dotGridGradient( Vector3(x0, y0, z1), pos);
	n1 = dotGridGradient( Vector3(x1, y0, z1), pos);
	ix0 = interpolate(n0, n1, sx);

	n0 = dotGridGradient(Vector3(x0, y1, z1), pos);
	n1 = dotGridGradient( Vector3(x1, y1, z1), pos);
	ix1 = interpolate(n0, n1, sx);

	value2 = interpolate(ix0, ix1, sy);
	float final = interpolate(value1, value2, sz);

	return final;
}
void chunknoisemap::addlayer(float scale, float intensity)
{
	maxint += intensity;
	
	int ind = 0;

	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			for (int z = 0; z < 16; z++)
			{
				Coord np = Vector3(x, y, z) + loc * 16;
				(*this)[Vector3(x, y, z)] += intensity * interpolatenoisemap(np.x,np.y,np.z,scale);
				ind++;
			}
		}
	}
	
}

void chunknoisemap::destroy()
{

	noisemap.destroy();
}
