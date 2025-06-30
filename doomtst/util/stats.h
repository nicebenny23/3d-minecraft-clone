#pragma once
#include "mathutil.h"
#include <algorithm>
#include <stdexcept>
#include "dynamicarray.h"

namespace statistics {
	//transforms a data set into the uniform distribution ranging from [-1,1]
	struct HistogramEqualizer {
		HistogramEqualizer() {
			 minvalue = 0;
			maxvalue = 0;
		}
		void destroy() {

			
			equalizedDistribution.destroy();
		}

		HistogramEqualizer(Cont::array<float> data) {
			const int DataPoints = data.length;
			
			//you need specific input patams as when its used for a finite dataset their are almost always points outside the range

			if (DataPoints == 0) {
				throw std::invalid_argument("Data cannot be empty.");
			}
			//nan for a Sentenal value
			equalizedDistribution = Cont::array<float>(DataPoints);
			for (int i = 0; i < DataPoints; i++)
			{
				equalizedDistribution[i] = NAN;
			}
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

			// Fill gaps in distribution as 2 elements could be mapped to the same bucket
			//starts at the smallest values and replaces every unfilled element with the largest value found sofar 
			float lastValue = -1.0f;
			for (int i = 0; i < DataPoints; i++) {
				if (!std::isnan(equalizedDistribution[i])) {
					lastValue = equalizedDistribution[i];
				}
				else {
					equalizedDistribution[i] = lastValue;
				}
			}
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

			return equalizedDistribution[get_bucket(value)];
			
		}

	private:
	
		Cont::array<float> equalizedDistribution;
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