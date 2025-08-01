#include "dynamicarray.h"
#pragma once
namespace util{

	struct counter {
		counter():current(0) {

		}
		size_t next() {
			return current++;
		}
		private:
		size_t current;
		size_t operator()() { 
			return current++; 
		}
		void reset()
		{
			current = 0;
		}
	};
}