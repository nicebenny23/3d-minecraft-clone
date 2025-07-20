#include "type_index.h"
#include "dynpool.h"
#include "queue.h"
#pragma once
template <typename T>
struct IntStore{

	stn::queue<T*> elem;
};
template <typename T>
struct store{
	stn::array<<dynPool::flux<T>>;

};