#include "dynamicarray.h"
#pragma once
namespace stn {
	template<typename T>
	struct ticks {
		ticks():current(0) {

		}

		void tick() {
			current++;
		}
		bool operator==(const ticks& other) const = default;
		bool operator!=(const ticks& other) const = default;

	private:
		size_t current;
	};

}