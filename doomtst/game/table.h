
#pragma once
#include "../util/dynpool.h"
#include "Id.h"
#include "../util/layout.h"

//virtual table of elements
template <typename T>
class virtual_table {
	virtual_table(stn::memory::layout layout, std::uint32_t rows) :pool(layout.size, layout.alignment), list(rows) {
		if (!layout.fits(stn::memory::layout_of<T>))
		{
			throw std::logic_error("Layout of a table must be repredstive of a virtual");
		}

	}
	T* operator[](std::uint32_t index) {
		if (!contains(index))
		{
			throw std::logic_error("Attempeted to acess invalid table element");
		}
		return list[index];
	}

	const T* operator[](std::uint32_t index) {
		if (!contains(index))
		{
			throw std::logic_error("Attempeted to acess invalid table element");
		}
		return list[index];
	}
	bool contains(std::uint32_t index) const{
		return list[index] != nullptr;
	}
	T* add(std::uint32_t index) {
		if (contains(index))
		{
			throw std::logic_error("Cannot Allocate to an already filled index");
		}
		list[index] = pool.alloc();
		//placment new somwhere
	}

	void remove(std::uint32_t index) {
		if (!contains(index))
		{
			throw std::logic_error("Attempeted to acess invalid table element");
		}
		pool.free(list[index]);
		list[index] = nullptr;
	}
	std::uint32_t rows() const{
		return list.length();
	}
	
	private:
	dynPool::flux<T> pool;
	stn::array<T*> list;

};