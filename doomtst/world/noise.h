#include <math.h>
#include "../util/random.h"
#include "../util/vector2.h"
#include <cmath>
#include "../util/dynamicarray.h"
#include "../util/vector3.h"
#ifndef noise_HPP
#define noise_HPP
using namespace v3;
using namespace dynamicarray;
struct pointmap
{

    array<Vector3> dirmap;
    v3::Coord loc;
    pointmap(v3::Coord location,float scl );

    Vector3& at(Coord pos);
};
struct chunknoisemap
{
   dynamicarray::array<float> noisemap;
    chunknoisemap(v3::Coord location);
   float& operator[](int ind);
   float& operator[](Coord pos);
    void addlayer(float scale, float intensity);
    void destroy();
    v3::Coord loc;
   
};

inline float interpolate(float v, float v1, float w) {
    w = (6*w*w- 15*w + 10)*w*w*w;
    return (v -v1)* w + v1;
   
}


inline v3::Vector3 randompointonsphere(Coord pnt) {
    v3::Vector3 retpoint = unitv;
    int l = 0;
    while (true) {
        retpoint = (randomseeded(pnt.x + l, pnt.y, pnt.z) - unitv / 2) * 2;


        if (magnitude2(retpoint) < 1)
        {
            return retpoint / magnitude(retpoint);
        }
        l += 1040;
    }
}
    
inline float dotGridGradient(Vector3 pos, float x, float y,float z) {

    Vector3 gradient =randompointonsphere(pos);

   
    float dx = x - pos.x;
    float dy = y - pos.y;
    float dz = z - pos.z;
    return (dx * gradient.x + dy * gradient.y+ dz * gradient.z);
}


inline float perlin(float x, float y,float z) {
    
    int x0 = (int)floor(x);
    int x1 = x0 + 1;
    int y0 = (int)floor(y);
    int y1 = y0 + 1;
    int z0 = (int)floor(z);
    int z1 = z0 + 1;
  
    float sx = x - (float)x0;
    float sy = y - (float)y0;
    float sz = z - (float)z0;
   
    float n0, n1, ix0, ix1, value1,value2;

    n0 = dotGridGradient(Vector3(x0, y0,z0), x, y,z);
    n1 = dotGridGradient(Vector3(x1, y0, z0), x, y,z);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(Vector3(x0, y1, z0), x, y,z);
    n1 = dotGridGradient(Vector3(x1, y1, z0), x, y,z);
    ix1 = interpolate(n0, n1, sx);

    value1 = interpolate(ix0, ix1, sy);
    n0 = dotGridGradient(Vector3(x0, y0, z1), x, y, z);
    n1 = dotGridGradient(Vector3(x1, y0, z1), x, y, z);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(Vector3(x0, y1, z1), x, y, z);
    n1 = dotGridGradient(Vector3(x1, y1, z1), x, y, z);
    ix1 = interpolate(n0, n1, sx);

    value2 = interpolate(ix0, ix1, sy);
    return interpolate(value1,value2,sz);
}
inline chunknoisemap* trueperlin(Coord chunk) {
   
    float inten = 1;
    chunknoisemap* map = new chunknoisemap(chunk);
    float todob = 1;
    float val = 0;
    int maxnum = 0;
    
    for (int i = 0; i < 6; i++)
    {
      
            map->addlayer( inten,todob);
        
        maxnum += todob;
        todob *= 1.1;
 
        inten /= 2;
    }
    val /= maxnum;
    return map;
}
#endif // !noise_HPP
