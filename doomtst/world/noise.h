#include <math.h>
#include "../util/random.h"
#include "../util/vector2.h"
#include <cmath>
#include "../util/dynamicarray.h"
#include "../util/vector3.h"
#include "../util/algorthm.h"
#include "../util/stats.h"
#pragma once
constexpr v3::Vec3 NoiseOffset = v3::Vec3(.838f, .38f, .49f);
constexpr float test_scale = 2.71818281828f;

using namespace v3;
using namespace stn;


struct noiseparams
{
    int distributionsize;
    noiseparams() {
        distributionsize = 400;
        octaves = 0;
        scalefactor = 0;
        startscale = 0;
        amplificationfactor = 0;
     
    }
    float scalefactor;    // Controls how scale is multiplied across octaves
    float startscale;       // Initial scale value for the noise
    int octaves;            // Number of octaves used in the noise generation
    float amplificationfactor;// Controls how amplitude 
    noiseparams(float startingscale, float scalemultiplyer, unsigned int noiseoctaves, float ampfactor) {
        distributionsize = 400;
        startscale = startingscale;
        scalefactor = scalemultiplyer;
        octaves = noiseoctaves;
        amplificationfactor = ampfactor;
    }
};

struct noisemap
{
    void createdist();
    noiseparams properties;
    
    noisemap();
    //evaluates Noise At pos
    float Eval(Vec3 pos);
 //evaluates Noise At pos
    float Eval(Coord pos);
   
    statistics::HistogramEqualizer equalizer;
    void create();
    void destroy();
    
};

float interpolatenoisemap(float x, float y, float z);
float EvaluateNoiseAtPoint(Vec3 point, noiseparams params);

inline void noisemap::createdist() {
    
    array<float> distribution = array<float>();
    
    float ScaleRange =properties.startscale * pow(properties.scalefactor, properties.octaves)* test_scale;

for (int i = 0; i < properties.distributionsize; i++)
{
    
    Vec3 randomPoint =NoiseOffset +randompointonsphere(0,i,0)/ScaleRange;
    float val = EvaluateNoiseAtPoint(randomPoint, properties);
    distribution.push(val);
}
equalizer= statistics::HistogramEqualizer(array<float>(distribution));
}
//higher scale means higher frequency or more chaotic
inline noisemap* genperlin(size_t octaves,  float frequencymul, float startfrequency, float ampmul) {
    noisemap* map = new noisemap();
    map->properties = noiseparams(startfrequency, frequencymul, octaves, ampmul);

    map->create();


    return map;
}
