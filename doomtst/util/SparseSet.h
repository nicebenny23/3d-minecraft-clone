#include "dynamicarray.h"
#include "robinHood.h"
#include <iterator>
#include <utility>
#include <type_traits> 
#include <limits>
#pragma once
namespace Sparse {
	struct Identity {
		template <typename U>
		constexpr U&& operator()(U&& t) const noexcept {
			return std::forward<U>(t);
		}
	};
	template <typename T, typename Hasher = std::hash<T>, typename KeyMap = Identity>
	struct PackedSet
	{

		using KeyType = std::decay_t<decltype(std::declval<KeyMap>()(std::declval<const T&>()))>;
		ska::flat_hash_map<KeyType, uint32_t, Hasher> sparse;
		Cont::array<T> dense;

		PackedSet() :sparse(), dense() {
		}

		// Element -> Key
		KeyType toKey(const T& elem) const {
			return  KeyMap{}(elem);
		}

		// Size of array
		size_t size() const {
			return dense.length;
		}

		using iterator = typename Cont::array<T>::iterator;

		iterator begin() { return dense.begin(); }
		iterator end() { return dense.end(); }

		// Adds an element
		void push(T&& elem) {
			auto [Loc, inserted] = sparse.insert({ toKey(elem), dense.length });
			if (inserted) {
				dense.push(std::move(elem));
			}
		}

		// Removes element at index
		void erase(const uint32_t index) {
			std::swap(dense[index], dense[dense.length - 1]);
			sparse.erase(toKey(dense[dense.length - 1]));
			// flips
			sparse[toKey(dense[index])] = index;
			dense.pop();
		}

		// Erases element
		void erase(const T& elem) {
			erase(indexOf(elem));
		}

		// Erases key
		void eraseKey(const KeyType& key){
			erase(keyIndex(key));
		}

		// If it contains element
		bool contains(const T& elem) const {
			return containsKey(toKey(elem));
		}

		// If it contains key
		bool containsKey(const KeyType& key) const {
			auto it = sparse.find(key);
			return (it != sparse.end());
		}

		// Returns the index of the key
		size_t keyIndex(const KeyType& key) const{
			auto it = sparse.find(key);
			if (it == sparse.end()) throw std::out_of_range("Element not found");
			return it->second;
		}

		// Returns the index of element
		size_t indexOf(const T& elem) const {
			return keyIndex(toKey(elem));
		}

		void clear() {
			dense.destroy();
			sparse.clear();
		}

		T& operator[](size_t index) {
			return dense[index];
		}

		T& getByKey(const KeyType& key) {
			return dense[sparse[key]];
		}

		T getOrDefault(const KeyType& key) const {
			auto it = sparse.find(key);
			if (it != sparse.end()) {
				return dense.list[it->second];
			}
			return T(); 
		}
	}; 
	static constexpr size_t ind_none= std::numeric_limits<size_t>::max();
	template<typename T, typename KeyMap = Identity>
	class SparseSet {
		Cont::array<size_t> sparse;
		Cont::array<T> dense;

	public:
	
		void push(const T& elem) {
			size_t key = KeyMap(elem);
			sparse[key] = dense.length;
			dense.push(elem);
		}
		void erase_key(size_t key) {
			size_t index = sparse[key];
			std::swap(dense[index], dense[dense.length - 1]);
			size_t moved_key = getID(dense[index]);
			sparse[moved_key] = index;
			dense.pop();
			sparse[key] = ind_none;
		}
		void erase(const T& elem) {
			erase_key(getID(elem));
		}
		bool contains(const T& elem) const {
			return contains_key(getID(elem));
		}


		bool contains_key(size_t key) const {
			return key < sparse.length && sparse[key] != ind_none;
		}

		size_t length() const {
			return dense.length;
		}
		
		void clear() {
			dense.destroy();
			sparse.clear();
		}
		using iterator = typename Cont::array<T>::iterator;

		iterator begin() { return dense.begin(); }
		iterator end() { return dense.end(); }
	private:
		size_t getID(const T& elem) {
			return KeyMap(elem);
		}
	};
}
