#include "noise.h"
using namespace dynamicarray;
chunknoisemap::chunknoisemap(v3::Coord location)
{

	loc = location;
	noisemap = dynamicarray::array<float>(16*16*16);
	for (int i = 0; i < 16*16*16; i++)
	{
		noisemap[i] = 0;
	}
}

Vector3& pointmap::at(Coord pos)
{
	return dirmap[pos.z+ 17 * pos.y+ 189 * pos.x];
}

float& chunknoisemap::operator[](int ind)
{
	return noisemap[ind];
}

float& chunknoisemap::operator[](Coord pos)
{
	return noisemap[pos.z+16*pos.y+256*pos.z];
}


float dotGridGradient(pointmap& map, Coord chunkpos, Vector3 pos) {

	Vector3 gradient = map.at( chunkpos);


	float dx = pos.x - chunkpos.x;
	float dy = pos.y- chunkpos.y;
	float dz = pos.z - chunkpos.z;
	return (dx * gradient.x + dy * gradient.y + dz * gradient.z);
}


float interpolatenoisemap(pointmap& map, float x, float y, float z){

	int x0 = floor(x);
	int x1 = x0 + 1;
	int y0 = floor(y);
	int y1 = y0 + 1;
	int z0 =floor(z);
	int z1 = z0 + 1;
	Vector3 pos = Vector3(x, y, z);
	float sx = x - x0;
	float sy = y - y0;
	float sz = z - z0;
	float n0, n1, ix0, ix1, value1, value2;
	n0 = dotGridGradient(map,Vector3(x0, y0, z0),pos);
	n1 = dotGridGradient(map, Vector3(x1, y0, z0), pos);
	ix0 = interpolate(n0, n1, sx);

	n0 = dotGridGradient(map, Vector3(x0, y1, z0), pos);
	n1 = dotGridGradient(map, Vector3(x1, y1, z0), pos);
	ix1 = interpolate(n0, n1, sx);

	value1 = interpolate(ix0, ix1, sy);
	n0 = dotGridGradient(map, Vector3(x0, y0, z1), pos);
	n1 = dotGridGradient(map, Vector3(x1, y0, z1), pos);
	ix0 = interpolate(n0, n1, sx);

	n0 = dotGridGradient(map, Vector3(x0, y1, z1), pos);
	n1 = dotGridGradient(map, Vector3(x1, y1, z1), pos);
	ix1 = interpolate(n0, n1, sx);

	value2 = interpolate(ix0, ix1, sy);
	return interpolate(value1, value2, sz);

}
void chunknoisemap::addlayer(float scale, float intensity)
{
	
	pointmap map = pointmap(loc, scale);
	int ind = 0;
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			for (int z = 0; z < 16; z++)
			{
				
				noisemap[ind] += intensity*interpolatenoisemap(map, (x)*scale+.13034390, y*scale + .1034390, z*scale + .5034390);
				ind++;
			}
		}
	}
	map.dirmap.destroy();
}

void chunknoisemap::destroy()
{

	noisemap.destroy();
}

pointmap::pointmap(v3::Coord location,float scl)
{
	int ind = 0;
	dirmap = array<Vector3>(17 * 17 * 17);
	loc = location;
	for (int x = 0; x < 17; x++)
	{
		for (int y = 0; y < 17; y++)
		{
			for (int z = 0; z < 17; z++)
			{
				Vector3 pos = location*16 +Coord(x, y, z);
				dirmap[ind] = randompointonsphere(pos * scl);
				ind++;
			}
		}
	}
}
