#pragma once
#include "mathutil.h"
#include <algorithm>
#include <stdexcept>
#include "../util/dynamicarray.h"
#include "../util/pair.h"
namespace statistics {
	template<typename T>
	concept RealEvaluatable = std::same_as<stn::result_type_t<decltype(&T::operator())>, double>;
	template<typename T>
	concept Bounded = requires(const T & f) {
		{
			f.range()
		} -> std::same_as<math::bounds>;
	};
	
	template<typename T>
	concept IndexedSampleable =
		requires(const T & d, size_t i) {
			{
				d.sample(i)
			} -> std::same_as<double>;
	};
	template<typename T>
	concept Distribution = RealEvaluatable<T>&&Bounded<T>;
	template<typename T>
	concept IndexedDistribution = Distribution<T>&&IndexedSampleable<T>;


	inline stn::array<double> interpolate(const stn::array<stn::Option<double>>& points, math::bounds bounds) {

		if (points.empty()) {
			return stn::array<double>();
		}
		stn::array<stn::indexed<double>> filled;
		if (points.first().is_none()) {
			filled.emplace(bounds.min(), 0);
		}
		for (size_t i = 0; i < points.length(); i++) {
			if (points[i]) {
				filled.emplace(points[i].unwrap(), i);
			}
		}
		if (points.last().is_none()) {
			if (points.first().is_none() && points.length() == 1) {
				filled.first().value = bounds.midpoint();
			}
			else {
				filled.emplace(bounds.max(), points.last_index());
			}
		}
		stn::indexed<double> min_val = filled.first();
		stn::indexed<double> max_val = filled.first();
		size_t inc = 0;
		stn::array<double> interpolation;
		for (size_t i = 0; i < points.length(); i++) {
			if (max_val.index == i) {
				min_val = max_val;
				if (inc != filled.last_index()) {
					inc++;
					max_val = filled[inc];
				}
			}
			size_t  diff = (max_val.index - min_val.index);
			if (diff != 0) {
				interpolation.push(stn::lerp(min_val.value, max_val.value, (i - static_cast<double>(min_val.index)) / diff));
			}
			else {
				interpolation.push(min_val.value);
			}
		}
		return interpolation;
	}
	//transforms a data set into the uniform distribution ranging from [-1,1]
	template<IndexedDistribution  T>
	struct HistogramEqualizer {
		void clear() {
			equalized.clear();
		}
		HistogramEqualizer(const T& value, size_t point_count) :value(value), samples(point_count) {
			compute_samples();
		}
		math::bounds range() const {
			return value.range();
		}
		void compute_samples() {
			if (samples == 0) {
				throw std::invalid_argument("Data cannot be empty.");
			}
			if (samples == 1) {
				throw std::invalid_argument("Data needs more than one sample");
			}
			stn::array<Option<double>> points = stn::array<Option<double>>(samples);
			stn::array<double> values;
			for (size_t i = 0; i < samples; i++) {
				values.push(value.sample(i));
			}
			std::sort(values.begin(), values.end());
			//find the bucket the datasets in this effecivvly inverts these values 
			for (size_t i = 0; i < samples; i++) {
				size_t bucket = get_bucket(values[i]);
				points.reach(bucket) = range().lerp(double(i) / double(samples - 1));
			}
			equalized = interpolate(points, range());

		}
		template<typename ...Args> requires std::invocable<T,Args&&...>
		double operator()(Args&&... args) const {
			return apply_distribution(value(std::forward<Args>(args)...));
		}
		// Applies the equalized distribution to a value
		double apply_distribution(double value) const {
			return equalized[get_bucket(value)];
		}
		T value;
	private:
		size_t samples;
		stn::array<double> equalized;
		// Maps a double value to a bucket index
		inline size_t get_bucket(double input_value) const {
			double normalized = range().unlerp_clamped(input_value);
			return int(normalized * (samples - 1));
		}

		// Float comparison function for qsort

	};

}