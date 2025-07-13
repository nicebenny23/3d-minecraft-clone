#include <math.h>
#include "../util/random.h"
#include "../util/vector2.h"
#include <cmath>
#include "../util/dynamicarray.h"
#include "../util/vector3.h"
#include "../util/algorthm.h"
#include "../util/stats.h"
#pragma once
#define NoiseOffset Vec3(.838f, .38f, .49f)
using namespace v3;
using namespace Cont;

enum noisetype {
    normalnoise = 0,
    rigid = 1,
    billowed = 2,
};

struct noiseparams
{
    int distributionsize;
    noiseparams() {
        distributionsize = 1000;
        octaves = 0;
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
        distributionsize = 1000;
        startscale = startingscale;
        scalefactor = scalemultiplyer;
        octaves = noiseoctaves;
        amplificationfactor = ampfactor;
        type = ntype;
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
    
    float ScaleRange =properties.startscale * pow(properties.scalefactor, properties.octaves)/100;

for (int i = 0; i < properties.distributionsize; i++)
{
    
    Vec3 randomPoint =NoiseOffset +randompointonsphere(0,i,0)/ScaleRange;
    float val = EvaluateNoiseAtPoint(randomPoint, properties);
    distribution.push(val);
}
equalizer= statistics::HistogramEqualizer(array<float>(distribution));
}


inline noisemap* genperlin(int octaves,  float scalemul, float startscale, float ampmul,noisetype type) {
    noisemap* map = new noisemap();
    map->properties = noiseparams(startscale, scalemul, octaves, ampmul,type);

    map->create();


    return map;
}
