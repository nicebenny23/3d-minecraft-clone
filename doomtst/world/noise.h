#include <math.h>
#include "../util/random.h"
#include "../util/vector2.h"
#include <cmath>
#include "../util/dynamicarray.h"
#include "../util/vector3.h"
#include "../util/algorthm.h"
#ifndef noise_HPP
#define noise_HPP
#define distrubutionsize 1000
using namespace v3;
using namespace dynamicarray;
extern array<v3::Vector3> seededdirections;

enum noisetype {
    normalnoise = 0,
    rigid = 1,
    billowed = 2,
};

struct noiseparams
{
    noiseparams() {
        octaves = -1;
        scalefactor = 0;
        startscale = 0;
        amplificationfactor = 0;
        type = normalnoise;
    }
    noisetype type;
    float scalefactor;    // Controls how scale is multiplied across octaves
    float startscale;       // Initial scale value for the noise
    int octaves;            // Number of octaves used in the noise generation
    float amplificationfactor;// Controls how amplitude 
    noiseparams(float startingscale, float scalemultiplyer, unsigned int noiseoctaves, float ampfactor,noisetype ntype) {

        startscale = startingscale;
        scalefactor = scalemultiplyer;
        octaves = noiseoctaves;
        amplificationfactor = ampfactor;
        type = ntype;
    }
};

void initrandomdirs();

struct noisemap
{
    dynamicarray::array<float> distribution;
    void createdist();
    noiseparams properties;
    
    noisemap();
    float operator[](Vector3 pos);
 
    float operator[](Coord pos);
   
  
    void create();
    void destroy();
    
 inline   float applydist(const float val);
};
inline float  interoplatequintic(const float& t) {

    return (6 * t*t- 15 * t + 10) * t *t * t;

}
inline float interpolate(const float & v, const float &v1, const float &w) {

    return (v1 - v) * w + v;

}

inline Vector3 randompointonsphere(int x, int y, int z);

float interpolatenoisemap(float x, float y, float z);
float computenoiseatpoint(Vector3 point, noiseparams params);

int comparefloat(const void* b, const void* a);


inline int getbucket(float value) {

    value += 1;
    value /= 2;
    value *= distrubutionsize;
    return clamp(int( value),0,distrubutionsize-1);
}
inline void noisemap::createdist() {
    
distribution = array<float>(distrubutionsize,false);
array<float> codistribution = array<float>(distrubutionsize, false);

    for (int i = 0; i < distrubutionsize; i++)
    {
        Vector3 tstpnt = Vector3(51.838, 1193.38 * i, -54.49) + randompointonsphere(6,  1000 * i, 90);
     
            float val = computenoiseatpoint(tstpnt, properties);
            codistribution[i] = val;
            distribution[i] = 0;
    }
    std::qsort(codistribution.list, codistribution.length, sizeof(float), comparefloat);
  
    for (int i = 0; i < distrubutionsize; i++)
    {
        distribution[getbucket(codistribution[i])] = 2 * float(i) /float( distrubutionsize )- 1.f;
    }
    //removing gaps
    float lastValue = -1; // Start with the minimum value
    for (int i = 0; i < distrubutionsize; i++) {
        if (distribution[i] != 0) {
            lastValue = distribution[i];
        }
        else {
            distribution[i] = lastValue;
        }
    }
    codistribution.destroy();
}


inline noisemap* genperlin(int octaves,  float scalemul, float startscale, float ampmul,noisetype type) {
    noisemap* map = new noisemap();
    map->properties = noiseparams(startscale, scalemul, octaves, ampmul,type);

    map->create();


    return map;
}
#endif // 
