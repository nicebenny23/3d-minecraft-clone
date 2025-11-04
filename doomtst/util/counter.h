#include "dynamicarray.h"
#pragma once
namespace stn {
	template<typename T>
	struct counter {
		counter():current(0) {

		}

		T next() {
			return T(current++);
		}
		void clear()
		{
			current = 0;
		}
		T value() const{
			return T(current);
		}
		private:
		size_t current;
		
		
	};
}