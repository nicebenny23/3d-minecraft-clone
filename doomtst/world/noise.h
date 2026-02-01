#pragma once
#include <cmath>
#include "../util/random.h"
#include "../util/dynamicarray.h"
#include "../math/vector3.h"
#include "../math/stats.h"
#include "../math/mathutil.h"

using namespace v3;
using namespace stn;

inline int PrimeX = 501125321;
inline int PrimeY = 1136930381;
inline int PrimeZ = 1720413743;
struct noise_parameters {



	noise_parameters(size_t octave_count, double frequency_multiplier, double initial_frequency, double amplification_multiplier) {
		histogram_sample_count = 400;
		starting_frequency = initial_frequency;
		frequency_factor = frequency_multiplier;
		octaves = octave_count;
		amplification_factor = amplification_multiplier;
	}

	double starting_frequency;
	size_t octaves;
	double frequency_factor;
	double amplification_factor;
	size_t histogram_sample_count;

	double final_frequency() const {
		return  starting_frequency * pow(frequency_factor, octaves - 1);
	}
	Point3 uniform_point(size_t i) const {
		return (Point3(i, 0, 0) + random::spherical(0, i, 0)) / final_frequency();
	}
};

inline double dot_grid_gradient(int x, int y, int z, double xd, double yd, double zd) {
	Vec3 pos = random::spherical(x, y, z);
	return xd * pos.x + yd * pos.y + zd * pos.z;
}

inline double interpolate_noise_map(double x, double y, double z) {
	int x0 = FastFloor(x);
	int y0 = FastFloor(y);
	int z0 = FastFloor(z);

	double xd0 = (double)(x - x0);
	double yd0 = (double)(y - y0);
	double zd0 = (double)(z - z0);
	double xd1 = xd0 - 1;
	double yd1 = yd0 - 1;
	double zd1 = zd0 - 1;

	double xs = interoplate_quintic(xd0);
	double ys = interoplate_quintic(yd0);
	double zs = interoplate_quintic(zd0);

	x0 *= PrimeX;
	y0 *= PrimeY;
	z0 *= PrimeZ;
	int x1 = x0 + PrimeX;
	int y1 = y0 + PrimeY;
	int z1 = z0 + PrimeZ;
	double n0, n1, ix0, ix1, value1, value2;
	n0 = dot_grid_gradient(x0, y0, z0, xd0, yd0, zd0);

	n1 = dot_grid_gradient(x1, y0, z0, xd1, yd0, zd0);
	ix0 = lerp(n0, n1, xs);

	n0 = dot_grid_gradient(x0, y1, z0, xd0, yd1, zd0);
	n1 = dot_grid_gradient(x1, y1, z0, xd1, yd1, zd0);
	ix1 = lerp(n0, n1, xs);

	value1 = lerp(ix0, ix1, ys);
	n0 = dot_grid_gradient(x0, y0, z1, xd0, yd0, zd1);
	n1 = dot_grid_gradient(x1, y0, z1, xd1, yd0, zd1);
	ix0 = lerp(n0, n1, xs);

	n0 = dot_grid_gradient(x0, y1, z1, xd0, yd1, zd1);
	n1 = dot_grid_gradient(x1, y1, z1, xd1, yd1, zd1);
	ix1 = lerp(n0, n1, xs);

	value2 = lerp(ix0, ix1, ys);
	return lerp(value1, value2, zs);
}


inline double perlin_for(Point3 point, noise_parameters params) {
	double noise_value = 0;
	double maxintensity = 0;
	double intensity = 1;

	double scale = params.starting_frequency;
	for (int i = 0; i < params.octaves; i++) {
		Point3 testpoint = point * scale;
		noise_value+=intensity * interpolate_noise_map(testpoint.x, testpoint.y, testpoint.z);
		maxintensity += intensity;
		scale *= params.frequency_factor;
		intensity *= params.amplification_factor;
	}
	return noise_value / maxintensity;
}

struct NoiseMap {
	
	NoiseMap(noise_parameters noise_properties) :properties(noise_properties) {
		array<double> distribution = array<double>();
		for (int i = 0; i < properties.histogram_sample_count; i++) {
			distribution.push(perlin_for(properties.uniform_point(i), properties));
		}
		equalizer = statistics::HistogramEqualizer(array<double>(distribution));
	}
	inline double operator()(Coord pos) {
		return this->operator()(Point3(pos));
	}
	inline double operator()(Point3 pos) {
		return equalizer.apply_distribution((perlin_for(pos, properties)));
	}
	inline void clear() {
		equalizer.clear();
	}

	private:
		noise_parameters properties;
		statistics::HistogramEqualizer equalizer;
};
