#pragma once
#include "mathutil.h"
#include <algorithm>
#include <stdexcept>
#include "../util/dynamicarray.h"
#include "../util/pair.h"
namespace statistics {

	inline stn::array<double> interpolate(stn::array<stn::Option<double>>& points, double min, double max) {

		if (points.empty()) {
			return stn::array<double>();
		}
		stn::array<stn::pair<size_t, double>> filled;
		if (points.first().is_none()) {
			filled.push(stn::pair(size_t(0), min));
		}
		for (size_t i = 0; i < points.length(); i++) {
			if (points[i]) {
				filled.push(stn::pair(i, points[i].unwrap()));
			}
		}
		if (points.last().is_none()) {
			if (points.first().is_none() && points.length() == 1) {
				filled.first().second = (min + max) / 2;
			}
			else {
				filled.push(stn::pair(points.last_index(), max));
			}
		}
		stn::pair<size_t, double> min_val = filled.first();
		stn::pair<size_t, double> max_val = filled.first();
		size_t inc = 0;
		stn::array<double> interpolation;
		for (size_t i = 0; i < points.length(); i++) {

			if (max_val.first == i) {

				min_val = max_val;
				if (inc!=filled.last_index()) {
					inc++;
					max_val = filled[inc];
				}
			}
			size_t  diff = (max_val.first - min_val.first);
			if (diff != 0) {
				interpolation.push(lerp(min_val.second, max_val.second, (i - static_cast<double>(min_val.first)) / diff));
			}
			else {
				interpolation.push(min_val.second);
			}
		}
		return interpolation;
	}
	//transforms a data set into the uniform distribution ranging from [-1,1]
	struct HistogramEqualizer {
		HistogramEqualizer() {
			minvalue = 0;
			maxvalue = 0;
			point_count= 0;
		}
		void clear() {
			equalizedDistribution.clear();
		}

		HistogramEqualizer(stn::array<double> data):point_count(data.length()){
			const size_t DataPoints = data.length();

			//you need specific input patams as when its used for a finite dataset their are almost always points outside the range

			if (DataPoints == 0) {
				throw std::invalid_argument("Data cannot be empty.");
			}
			//nan for a Sentenal value
			stn::array<Option<double>> points = stn::array<Option<double>>(DataPoints);

			std::sort(data.begin(), data.end(), [](double a, double b) {
				return a < b;
				});
			minvalue = data.first();
			maxvalue = data.last();

			// Prevent division by zero
			if (minvalue == maxvalue) {
				maxvalue += 1.0f;
			}


			//find the bucket the datasets in this effecivvly inverts these values 
			for (int i = 0; i < DataPoints; i++) {
				int bucket = get_bucket(data[i]);
				//the function 2x-1 transforms the interval [0,1] to [-1,1]
				points.reach(bucket)= 2.0f * double(i) / double(DataPoints) - 1.0f;
			}
			equalizedDistribution= interpolate(points, -1, 1);
		}
		bool generated() {

			return equalizedDistribution.length() > 0;
		}
		// Applies the equalized distribution to a value
		double apply_distribution(double value) {
			if (!generated()) {
				throw std::logic_error("Distribution has not been generated");
			}
			return equalizedDistribution[get_bucket(value)];
		}

	private:
		//since we need get bucket to work consistently
		size_t point_count;

		stn::array<double> equalizedDistribution;
		double minvalue;
		double maxvalue;
		// Maps a double value to a bucket index
		inline int get_bucket(double value) const {
			double normalized = (value - minvalue) / (maxvalue - minvalue);
			normalized = clamp(normalized, 0.0f, 1.0f);
			int index = int(normalized * (point_count -1));
			return clamp(index, 0, (point_count-1));
		}

		// Float comparison function for qsort

	};

}