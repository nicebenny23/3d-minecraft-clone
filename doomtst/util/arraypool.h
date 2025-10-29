#include "dynpool.h"
#pragma once
namespace dyn_allocator {
	template <typename T>
	struct dyn_alloc
	{
		stn::array<dynPool::flux> pools;
	void tranfer() {
	


	}
	T* alloc(size_t block_pow) {

		if (pools.length()<= block_pow)
		{
			for (size_t i = pools.length(); i < block_pow; i++)
			{
				pools.reach(i) = dynPool::flux<T>(1 << i);
			}
			
		}
		return pools[block_pow].alloc();
	}
	
	void free(T* ptr) {

		pools[block_pow].free(ptr);

	}
	T* realloc()


	};
}