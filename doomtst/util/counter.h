#include "dynamicarray.h"
#pragma once
namespace stn {

	struct counter {
		counter():current(0) {

		}

		size_t next() {
			return current++;
		}
		void clear()
		{
			current = 0;
		}
		size_t value() const{
			return current;
		}
		private:
		size_t current;
		
		
	};
}