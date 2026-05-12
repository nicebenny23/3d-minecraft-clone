#pragma once
#include "random.h"


uint64_t seed64 = 0;
namespace random {
	inline void randomize_64(uint64_t& seed_64) {

		seed64 ^= seed64 << 13;
		seed64 ^= seed64 >> 7;
		seed64 ^= seed64 << 17;

	}
	float random() {
		randomize_64(seed64);
		return static_cast<double>(seed64) / UINT64_MAX;
	}
	bool randombool() {
		randomize_64(seed64);
		return seed64 % 2;
	}

	void randomcoord(unsigned int& seed) {
		seed ^= seed << 13;
		seed ^= seed >> 17;
		seed ^= seed << 5;
	}

	float random(float max) {

		return random() * max;
	}
	stn::array<v3::Vec3> seeded_directions;

	void InitRandomDirections() {
		const int startingseed = 5;
		const int randomizeiter = 10;
		unsigned int noiseval = startingseed;
		for (int i = 0; i < randomizeiter; i++) {
			randomcoord(noiseval);
		}

		seeded_directions = stn::array<v3::Vec3>();
		size_t ushort_amt = (1 + std::numeric_limits<unsigned short>().max());
		for (int u = 0; u < ushort_amt; u++) {

			v3::Vec3 PointOnCircle;
			do {

				for (size_t i = 0; i < 10; i++) {
					randomcoord(noiseval);
				}
				PointOnCircle.x = noiseval;
				for (size_t i = 0; i < 10; i++) {
					randomcoord(noiseval);
				}
				PointOnCircle.y = noiseval;
				for (size_t i = 0; i < 10; i++) {

					randomcoord(noiseval);
				}
				PointOnCircle.z = noiseval;
				PointOnCircle /= static_cast<float>(MAXUINT32);
				PointOnCircle -= v3::unitv / 2;
				PointOnCircle * 2;
			} while (mag2(PointOnCircle) > 1);
			seeded_directions.push(normal(PointOnCircle));
		}
	}
	

	v3::Vec3 spherical() {
		randomize_64(seed64);
		return seeded_directions.unchecked_at(seed64&std::numeric_limits<unsigned short>().max());
	}

	void initilize_random() {
		InitRandomDirections();
		seed64 = 1;
	}
}