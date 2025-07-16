#pragma once
#include "dynamicarray.h"
#include "robinHood.h"
#include <iterator>
#include <utility>
#include <type_traits> 
#include <unordered_map>
#include <limits>

namespace Sparse {



	struct Identity {
		template <typename U>
		constexpr const U& operator()(const U& t) const noexcept {
			return t;
		}
	};




	template <typename T, typename Hasher = std::hash<T>, typename KeyMap = Identity>
	struct PackedSet
	{

		using KeyType = std::decay_t<decltype(std::declval<KeyMap>()(std::declval<const T&>()))>;
		static_assert(std::is_invocable_r_v<KeyType, KeyMap, const T&>, "KeyMap must be callable with const T& and return KeyType");

		// 2. Hasher must be callable with KeyType and return size_t
		static_assert(std::is_invocable_r_v<size_t, Hasher, KeyType>,"Hasher must be callable with KeyType and return size_t");

		// 3. KeyType must be copy constructible (for flat_hash_map keys)
		static_assert(std::is_copy_constructible_v<KeyType>,"KeyType must be copy constructible");

		// 4. T must be move constructible (used in dense storage)
		static_assert(std::is_move_constructible_v<T>,"T must be move constructible");

	robin_hood::unordered_flat_map<KeyType, size_t, Hasher> sparse;
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
			void push(const T& elem) {
			auto [Loc, inserted] = sparse.insert({ toKey(elem), dense.length });
			if (inserted) {
				dense.push(std::move(elem));
			}
			}

		// Removes element at index
		void erase(const size_t index) {
			if (dense.length<=index)
			{
				throw std::logic_error("cannot delete out of bounds index");
			}
			size_t oth_index = dense.length-1;
			std::swap(dense[index], dense[oth_index]);
			sparse[toKey(dense[index])] = index;
			sparse.erase(toKey(dense[oth_index]));
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
	static constexpr size_t ind_none = static_cast<size_t>(-1); // Same as max, used as invalid index
	template<typename T, typename KeyMap = Identity>
	struct SparseSet {
		static_assert(std::is_invocable_r_v<size_t, KeyMap, const T&>,"KeyMap must take const T& and return a size_t index");
		Cont::array<size_t> sparse;
		Cont::array<T> dense;
	

		void push(const T& elem) {
			size_t key = getID(elem);
			sparse[key] = dense.length;
			dense.push(elem);
		}
		void erase_key(size_t key) {
			size_t index = sparse[key];
			size_t last_index = dense.length - 1;
			if (index != last_index) {
				
				std::swap(dense[index], dense[last_index]);
				//getds the dense id ->finds id in sparse map,indexes it so the index matches what sparse is suppoosed to map to  
				sparse[getID(dense[index])] = index;
			}
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
			sparse.destroy();
		}
		T& getByKey(const size_t key) {
			if (sparse[key]==ind_none)
			{
				throw std::logic_error("Element doesent exist");
			}
			return dense[sparse[key]];
		}
		using iterator = typename Cont::array<T>::iterator;

		iterator begin() { return dense.begin(); }
		iterator end() { return dense.end(); }
	
			 T getOrDefault(const size_t & key) {
				if (key < sparse.length && sparse[key] != ind_none) {
					return dense[sparse[key]];
				}
				return T();
			}
			
		T& operator[](size_t index) {
			return dense[index];
		}
	private:
		
		size_t getID(const T& elem) const {
			return KeyMap{}(elem);
		}
	};
}
