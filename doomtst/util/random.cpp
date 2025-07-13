#pragma once
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


float random(float max){

    return random() * max;
}

int randomint(int max){

    return (randomint()%max);
}
 Cont::array<v3::Vec3> seededdirections;

void InitRandomDirections()
{
	const int startingseed = 5;
	const int randomizeiter = 10;
	unsigned int noiseval = startingseed;
	for (int i = 0; i < randomizeiter; i++)
	{
		randomcoord(noiseval);
	}

	seededdirections = Cont::array<v3::Vec3>();
	for (int i = 0; i < USHRT_MAX; i++)
	{

		v3::Vec3 PointOnCircle;
		do
		{

			randomcoord(noiseval);
			randomcoord(noiseval);
			randomcoord(noiseval);
			PointOnCircle.x = noiseval;
			randomcoord(noiseval);
			randomcoord(noiseval);
			randomcoord(noiseval);
			PointOnCircle.y = noiseval;
			randomcoord(noiseval);
			randomcoord(noiseval);
			randomcoord(noiseval);
			PointOnCircle.z = noiseval;
			PointOnCircle /= static_cast<float>(MAXUINT32);
			PointOnCircle -= v3::unitv / 2;
			PointOnCircle * 2;
		} while (mag2(PointOnCircle) > 1);
		seededdirections.push(normal(PointOnCircle));
	}
}
int Hash(int seed, int xPrimed, int yPrimed, int zPrimed)
{
	int hash = seed ^ xPrimed ^ yPrimed ^ zPrimed;
	hash *= 0x27d4eb2d;
	return hash;
}

void initrandom(){
	InitRandomDirections();
    seed64 = 1;
}