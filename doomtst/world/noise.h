#pragma once
#include <cmath>
#include "../util/random.h"
#include "../util/dynamicarray.h"
#include "../math/vector3.h"
#include "../math/stats.h"
#include "../math/mathutil.h"
namespace math {

	
	struct WorleyNoise {

		double sample(size_t i) const {

			v3::Point3 sample_point = v3::Point3(0, 0, 0) + random::spherical(1, 1, 1, i) * 100;
			return this->operator()(sample_point, i);
		}
		double operator()(v3::Vec3 point, size_t seed) const {
			double min_dist = std::numeric_limits<double>().infinity();
			for (int x = -1; x<=int(1); x++) {
				for (int y = -1; y<=int(1); y++) {
					for (int z = -1; z <=int(1); z++) {
						v3::Coord sample =v3::Coord::from_vec3(v3::Vec3(x,y,z)+point);
						v3::Point3 pos = v3::Point3(sample)+ random::cubical(sample.x, sample.y, sample.z, seed);
						stn::set_min(min_dist, v3::dist(point,pos));
					}
				}
			}
			return min_dist;
		}
		math::bounds range() const {
			return math::bounds(0, 1);
		}
	};
	inline double dot_grid_gradient(int x, int y, int z, double xd, double yd, double zd,size_t seed) {
		v3::Vec3 pos = random::spherical(seed,x, y, z);
		return xd * pos.x + yd * pos.y + zd * pos.z;
	}
	
	inline double interpolate_noise_map(v3::Point3 point,size_t seed) {

		int PrimeX = 501125321;
		int PrimeY = 1136930381;
		int PrimeZ = 1720413743;
		int x0 = fast_floor(point.x);
		int y0 = fast_floor(point.y);
		int z0 = fast_floor(point.z);

		double xd0 = (double)(point.x - x0);
		double yd0 = (double)(point.y - y0);
		double zd0 = (double)(point.z - z0);
		double xd1 = xd0 - 1;
		double yd1 = yd0 - 1;
		double zd1 = zd0 - 1;

		double xs = math::interoplate_quintic(xd0);
		double ys = math::interoplate_quintic(yd0);
		double zs = math::interoplate_quintic(zd0);

		x0 *= PrimeX;
		y0 *= PrimeY;
		z0 *= PrimeZ;
		int x1 = x0 + PrimeX;
		int y1 = y0 + PrimeY;
		int z1 = z0 + PrimeZ;
		double n0, n1, ix0, ix1, value1, value2;
		n0 = dot_grid_gradient(x0, y0, z0, xd0, yd0, zd0, seed);

		n1 = dot_grid_gradient(x1, y0, z0, xd1, yd0, zd0, seed);
		ix0 = stn::lerp(n0, n1, xs);

		n0 = dot_grid_gradient(x0, y1, z0, xd0, yd1, zd0, seed);
		n1 = dot_grid_gradient(x1, y1, z0, xd1, yd1, zd0, seed);
		ix1 = stn::lerp(n0, n1, xs);

		value1 = stn::lerp(ix0, ix1, ys);
		n0 = dot_grid_gradient(x0, y0, z1, xd0, yd0, zd1, seed);
		n1 = dot_grid_gradient(x1, y0, z1, xd1, yd0, zd1, seed);
		ix0 = stn::lerp(n0, n1, xs);

		n0 = dot_grid_gradient(x0, y1, z1, xd0, yd1, zd1, seed);
		n1 = dot_grid_gradient(x1, y1, z1, xd1, yd1, zd1, seed);
		ix1 = stn::lerp(n0, n1, xs);

		value2 = stn::lerp(ix0, ix1, ys);
		return stn::lerp(value1, value2, zs);
	}
	struct Octave {
		double period;
		double weight;
	};
	struct OctaveSeries {
		size_t octaves=1;
		double starting_period;
		double period_factor=1;
		double starting_amplifcation=1;
		double amplification_factor=1;
		stn::array<Octave> generate() const {
			stn::array<Octave> result;
			
			double current_period = starting_period;
			double current_weight = starting_amplifcation;

			for (size_t i = 0; i < octaves; ++i) {
				result.push(Octave{ .period=current_period, .weight=current_weight });
				current_period *= period_factor;
				current_weight *= amplification_factor;
			}

			return result;
		}
	};
	struct PerlinNoise {

		PerlinNoise(const OctaveSeries& series) {
			octaves = series.generate();
		}
		stn::array<Octave> octaves;
		inline double operator()(v3::Point3 point,size_t seed) const {
			double noise_value = 0;
			double maxintensity = 0;
			for (std::size_t i = 0; i < octaves.length();i++) {
				const Octave& octave = octaves[i];
				v3::Point3 testpoint = point/octave.period;
				noise_value += octave.weight* interpolate_noise_map(testpoint+Vec3(.5,.5,.5), seed + 1000 * i);
				maxintensity += octave.weight;
			}
			return noise_value / maxintensity;
		}
		double period() const {
			if (octaves.length()==0) {
				return 0;
			}
			double per = 0;
			for (const Octave& octave : octaves) {
				stn::set_max(per, octave.period);
			}
			return per;
		}
		double sample(size_t i) const {
			
			v3::Point3 sample_point=v3::Point3(0,0,0)+random::spherical(1, 1, 1, i)*100*period();
			return this->operator()(sample_point,i);
		}
		math::bounds range() const {
			return math::bounds(-1,1);
		}
	};




	using WorleyNoiseMap = statistics::HistogramEqualizer<WorleyNoise>;
	using NoiseMap=statistics::HistogramEqualizer<PerlinNoise>;
}