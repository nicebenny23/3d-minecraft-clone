#include <math.h>
#include "../util/random.h"
#include "../util/vector2.h"
#include <cmath>
#include "../util/dynamicarray.h"
#include "../util/vector3.h"
#ifndef noise_HPP
#define noise_HPP
#define distrubutionsize 4096
using namespace v3;
using namespace dynamicarray;
extern array<v3::Vector3> seededdirections;


void initrandomdirs();

enum noisetype {
    normalnoise = 0,
    rigid = 1,
    billowed = 2,
};

struct noisemap
{
    dynamicarray::array<float> values;
    noisemap(v3::Coord location,Coord chksize);
    float& operator[](Vector3 pos);
    float operator[](int ind);
    float operator[](Coord pos);
    Coord size;
    float& at(int ind);
    void addlayer(float scale, float intensity,noisetype type);
    void destroy();
    v3::Coord loc;
    float maxint;
};

inline float interpolate(float v, float v1, float w) {

     w = (6*w*w- 15*w + 10)*w*w*w;
    return (v1 - v) * w + v;

}


inline v3::Vector3 randompointonsphere(Coord pnt) {
    return(seededdirections.fastat(randomushortfromdir(pnt.x, pnt.y, pnt.z)));
}

inline noisemap* genperlin(Coord chunk,int octaves,  float scalemul, float startscale, float ampmul,noisetype type) {

    float intensity = 1;
    noisemap* map = new noisemap(chunk,unitv*16);
    float scale = startscale;
    for (int i = 0; i < octaves; i++)
    {

        map->addlayer(scale, intensity,type);



        scale *= scalemul;


        intensity *= ampmul;
    }

    return map;
}

inline noisemap* genperlin2d(Coord chunk, int octaves, float scalemul, float startscale, float ampmul, noisetype type) {

    float intensity = 1;
    noisemap* map = new noisemap(chunk, Coord(16,1,16));
    float scale = startscale;
    for (int i = 0; i < octaves; i++)
    {

        map->addlayer(scale, intensity, type);



        scale *= scalemul;


        intensity *= ampmul;
    }

    return map;
}
#endif // 
