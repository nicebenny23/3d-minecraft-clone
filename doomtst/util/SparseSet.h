#include "dynamicarray.h"
#include <unordered_map>
#include <iterator>
#pragma once
namespace Sparse {
	template <typename T, typename Hasher = std::hash<T>>
	struct SparseSet
	{
		std::unordered_map<T, size_t, Hasher> sparse;
		Cont::array<T> dense;
		void push(const T& elem) {
			auto [Loc, inserted] = sparse.insert({ elem, dense.length });
			if (inserted)
			{
				dense.push(elem);
			}

		}
		void remove(size_t index) {
			std::swap(dense[index], dense[dense.length - 1]);
			sparse.erase(dense[dense.length - 1]);
			sparse[dense[index]] = index;
			dense.pop();
		}
		size_t size() const {
			return dense.length;
		}
		void remove(const T& elem) {
			remove(find(elem));
		}
		bool contains(const T& elem) const {
			auto it = sparse.find(elem);
			return (it != sparse.end());
		}
		size_t find(const T& elem) const {


			auto it = sparse.find(elem);
			if (it == sparse.end()) throw std::out_of_range("Element not found");
			return it->second;

		}
		using iterator = typename Cont::array<T>::iterator;

		iterator begin() { return dense.begin(); }
		iterator end() { return dense.end(); }

	
		SparseSet() :sparse(), dense() {

		}
		void Clear() {
			dense.clear();
			sparse.clear();
		}
		T& operator[](size_t index) {
			return dense[index];
		}
	};

}