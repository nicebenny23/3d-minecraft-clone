#include "random.h"

uint64_t val = 0;
float   random()
{
    //xorshift 64



    val ^= val << 13;
    val ^= val >> 7;
    val ^= val << 17;

    return static_cast<double>(val) / UINT64_MAX;

}
bool randombool() {
    val ^= val << 13;
    val ^= val >> 7;
    val ^= val << 17;

    return val % 2;
}

unsigned int randomint()
{

    val ^= val << 13;
    val ^= val >> 7;
    val ^= val << 17;

    return static_cast<double>(val) *UINT32_MAX/ UINT64_MAX;
}

void randomcoord(unsigned int& seed)
{
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;

    
}

v3::Vector3 randomseeded(unsigned int x, unsigned int y, unsigned int z)
{
   
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

float random(float max)
{

    val ^= val << 13;
    val ^= val >> 7;
    val ^= val << 17;

    return max * static_cast<double>(val) / UINT64_MAX;
}
template <typename T>

bool randombool(float truechance) {


    if (random(100) <= truechance)
    {
        return true;
    }
    return false;
}
int randomint(float max)
{

    return round(random(max) - .5);
}
void  randominit()
{
    int s = 1;

    val = s;

}