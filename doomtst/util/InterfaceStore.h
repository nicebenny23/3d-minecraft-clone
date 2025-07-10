#include "type_index.h"
#include "dynpool.h"
#include "queue.h"
#pragma once
template <typename T>
struct IntStore{

	Cont::queue<T*> elem;
};
template <typename T>
struct store{
	Cont::array<<dynPool::flux<T>>;

};