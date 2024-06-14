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
    int s = time(0);

    val = s;

}