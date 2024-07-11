#include <math.h>
#include "../util/random.h"
#include "../util/vector2.h"
#include <cmath>

#ifndef noise_HPP
#define noise_HPP



float interpolate(float v, float v1, float w) {
    w = (6*w*w- 15*w + 10)*w*w*w;
    return (v * w + v1*(1-w));
   
}


v3::Vector3 randompointonsphere(Coord pnt) {
    v3::Vector3 retpoint = unitv;
    int l = 0;
    while (1<magnitude(retpoint))
    {
        retpoint = (randomseeded(pnt.x+l,pnt.y-l,pnt.z)-unitv/2)*2;
        l += 1040;
    }
    return retpoint / magnitude(retpoint);
}
v2::Vector2 randomGradient(iv2::IntVector2 intvec) {
    
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;
    unsigned a = intvec.x, b = intvec.y;
    a *= 3284157443; b ^= a << s | a >> w - s;
    b *= 1911520717; a ^= b << s | b >> w - s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); 
    v2::Vector2 v;
    v.x = cos(random); v.y = sin(random);
    return v;
}
float dotGridGradient(Vector3 pos, float x, float y,float z) {

    Vector3 gradient =randompointonsphere(pos);

   
    float dx = x - pos.x;
    float dy = y - pos.y;
    float dz = z - pos.z;
    return (dx * gradient.x + dy * gradient.y+ dz * gradient.z);
}


float perlin(float x, float y,float z) {
    
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
float trueperlin(float x, float y,float z) {
    float todob = 1;
    float val = 0;
    int maxnum = 0;
    
    for (int i = 0; i < 6; i++)
    {
        if (i<2)
        {
            val += todob *std::min(.4f, std::max(float(-.4), perlin(x + .1415926, y + .590926, z + .6548916)));
        }

        else{
            val += todob *  perlin(x + .1415926, y + .590926, z + .6548916);
        }
        maxnum += todob;
        todob *= 1.1;
 
        x /= 2;
        y /= 2;
        z /= 2;
    }
    val /= maxnum;
    return val;
}
#endif // !noise_HPP
