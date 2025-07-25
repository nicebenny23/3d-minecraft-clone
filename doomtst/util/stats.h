#pragma once
#include "mathutil.h"
#include <algorithm>
#include <stdexcept>
#include "dynamicarray.h"
#include "pair.h"
namespace statistics {

	inline void interpolate(stn::array<float>& to_interpolate, float min, float max) {

		if (to_interpolate.length==0)
		{
			return;
		}
		stn::array<util::pair<size_t, float> > filled;
		if (std::isnan(to_interpolate.first()))
		{

			filled.push(util::pair(size_t(0), min));
		}
		for (size_t i = 0; i < to_interpolate.length; i++) {
			if (!std::isnan(to_interpolate[i])) {
				filled.push(util::pair(i, to_interpolate[i]));
			}
		}
		if (std::isnan(to_interpolate.last()))
		{
			if (std::isnan(to_interpolate.first())&&to_interpolate.length==1)
			{
				filled.first().second = (min + max)/ 2;
			}
			else {
				filled.push(util::pair(size_t(to_interpolate.length - 1), max));
			}
			}
	util::pair<size_t, float> min_val = filled.first();
	util::pair<size_t, float> max_val = filled.first();
	size_t inc = 0;
	for (int i = 0; i < to_interpolate.length; i++) {

		if (max_val.first == i) {
			inc++;
			min_val = max_val;
			if (inc!= filled.length)
			{

				max_val = filled[inc];
			}

		}
		int diff = (max_val.first - min_val.first);
		if (diff != 0)
		{
			to_interpolate[i] = lerp(min_val.second, max_val.second, (i - static_cast<float>(min_val.first)) / diff);
		}
		else
		{
			to_interpolate[i] = min_val.second;
		}

	}
	filled.destroy();

	}	
	//transforms a data set into the uniform distribution ranging from [-1,1]
	struct HistogramEqualizer {
		HistogramEqualizer() {
			 minvalue = 0;
			maxvalue = 0;
		}
		void destroy() {

			
			equalizedDistribution.destroy();
		}

		HistogramEqualizer(stn::array<float> data) {
			const size_t DataPoints = data.length;
			
			//you need specific input patams as when its used for a finite dataset their are almost always points outside the range

			if (DataPoints == 0) {
				throw std::invalid_argument("Data cannot be empty.");
			}
			//nan for a Sentenal value
			equalizedDistribution = stn::array<float>(DataPoints,NAN);
			
			std::sort(data.list, data.list + DataPoints, [](float a, float b) {
				return a < b;
				});
			minvalue = data[0];
			maxvalue = data[data.length-1];

			// Prevent division by zero
			if (minvalue == maxvalue) {
				maxvalue += 1.0f;
			}

			
			//find the bucket the datasets in this effecivvly inverts these values 
			for (int i = 0; i < DataPoints; i++) {
				int bucket = get_bucket(data[i]);
				//the function 2x-1 transforms the interval [0,1] to [-1,1]
				equalizedDistribution[bucket] = 2.0f * float(i) / float(DataPoints) - 1.0f;
			}
			
			
			interpolate(equalizedDistribution,-1,1);
	
			data.destroy();
		}
		bool generated() {

			return equalizedDistribution.length > 0;
}
		// Applies the equalized distribution to a value
		float apply_distribution(float value) {
			if (!generated()) {
				throw std::logic_error("Distribution has not been generated");
			}
			float l=equalizedDistribution[1];
			int d = 3;
			return equalizedDistribution[get_bucket(value)];
			
		}

	private:
	
		stn::array<float> equalizedDistribution;
		float minvalue;
		float maxvalue;
		// Maps a float value to a bucket index
		inline int get_bucket(float value) const {
			float normalized = (value - minvalue) / (maxvalue - minvalue);
			normalized = clamp(normalized, 0.0f, 1.0f);
			int index = int(normalized * (equalizedDistribution.length-1));
			return clamp(index, 0, equalizedDistribution.length- 1);
		}

		// Float comparison function for qsort
		
	};

}