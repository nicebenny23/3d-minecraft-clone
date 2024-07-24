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
extern array<v3::Vector3> seededdirections;
void initrandomdirs();
struct chunknoisemap
{
    dynamicarray::array<float> noisemap;
    chunknoisemap(v3::Coord location);
    float& operator[](Vector3 pos);
    float operator[](int ind);
    float operator[](Coord pos);
    float& at(int ind);
    void addlayer(float scale, float intensity);
    void destroy();
    v3::Coord loc;
    float maxint;
};

inline float interpolate(float v, float v1, float w) {

     w = (6*w*w- 15*w + 10)*w*w*w;
    return (v1 - v) * w + v;

}


inline v3::Vector3 randompointonsphere(Coord pnt) {
    return(seededdirections[randomushortfromdir(pnt.x, pnt.y, pnt.z)]);
}

inline chunknoisemap* trueperlin(Coord chunk, float startscale, float ampmul, float scalemul, int iter) {

    float intensity = 1;
    chunknoisemap* map = new chunknoisemap(chunk);
    float scale = startscale;
    for (int i = 0; i < iter; i++)
    {

        map->addlayer(scale, intensity);



        scale *= scale;


        intensity *= ampmul;
    }

    return map;
}
#endif // !noise_HPP