#include "random.h"

uint64_t seed64 = 0;
inline void randomize() {

    seed64 ^= seed64 << 13;
    seed64 ^= seed64 >> 7;
    seed64 ^= seed64 << 17;

}
float random(){
    randomize();
    return static_cast<double>(seed64) / UINT64_MAX;
}
bool randombool() {
    randomize();
    return seed64 % 2;
}

void randomcoord(unsigned int& seed){
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;


}

unsigned int randomint(){

    randomize();
    return (static_cast<double>(seed64) *UINT32_MAX)/ UINT64_MAX;
}

v3::Vector3 randomseeded(unsigned int x, unsigned int y, unsigned int z){
   
    // Use a mix of XOR and multiplication to combine the seeds
    x ^= y + 0x9e3779b97f4a7c15 + (x << 6) + (x >> 2);
    x ^= z + 0x9e3779b97f4a7c15 + (x << 6) + (x >> 2);
    x += (x << 3) + (x >> 7);
    x ^= (x << 11) ^ (x >> 5);
    v3::Vector3 point;
    point.x = static_cast<double>(x) / static_cast<float>(MAXUINT32);
    randomcoord(x);
  
    point.y = static_cast<double>(x) / static_cast<float>(MAXUINT32);
    randomcoord(x);
 
    point.z = static_cast<double>(x) / static_cast<float>(MAXUINT32);
    return point;
}

float random(float max){

    return random() * max;
}

int randomint(int max){

    return (randomint()%max);
}
void initrandom(){
    seed64 = 1;
}