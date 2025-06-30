#pragma once
#include "dynamicarray.h"
#include <unordered_map>
#include <iterator>
#pragma once
namespace Key {
	template <typename T, typename Key,auto KeyFunc>
	struct keySet
	{
		std::unordered_map<size_t, size_t> sparse;
		Cont::array<T> dense;
		void push(const T& elem) {
			size_t key = KeyFunc(elem);
			auto [Loc, inserted] = sparse.insert({key, dense.length });
			if (inserted)
			{
				dense.push(elem);
			}

		}
		size_t& get_index(const T& value) {
			return sparse[KeyFunc(value)];
		}

		size_t get_key(const size_t& value) {
			return KeyFunc(dense[value]);
		}
		void remove(size_t index) {
			size_t lastIndex = dense.length - 1;
			std::swap(dense[index], dense[lastIndex]);
			size_t k_end = get_key(lastIndex); // key of element to remove (at last position before pop)
			size_t k_ind = get_key(index);     // key of element swapped in (now at index)
			sparse.erase(k_end)
			if (index != lastIndex) {
				sparse[k_ind] = index;
			}
			dense.pop();
		}
		size_t size() const {
			return dense.length;
		}
		void remove(const T& elem) {
			remove(find(elem));
		}
		bool contains(const T& elem) const {
			auto it = sparse.find(KeyFunc(elem));
			return (it != sparse.end());
		}
		size_t find(const T& elem) const {
			auto it = sparse.find(KeyFunc(elem));
			if (it == sparse.end()) throw std::out_of_range("Element not found");
			return it->second;

		}

		using iterator = typename Cont::array<T>::iterator;

		iterator begin() { return dense.begin(); }
		iterator end() { return dense.end(); }

		T& operator[](size_t index) {
			return dense[index];
		}
	};

}