#pragma once
#include "dynamicarray.h"

namespace stn {
	//an array restricted with stack operations
	template<typename T>
	struct stack {
		stack() = default;

		bool empty() const {
			return list.empty();
		}
		bool non_empty() const {
			return list.non_empty();
		}

		T& peek() {
			if (empty()) throw std::out_of_range("Error: Attempted to peek an empty stack.");
			return list.last();
		}

		const T& peek() const {
			if (empty()) throw std::out_of_range("Error: Attempted to peek an empty stack.");
			return list.last();
		}

		T pop() {
			if (empty()) throw std::out_of_range("Error: Attempted to pop from an empty stack.");
			return list.pop();
		}

		void push(const T& val) {
			list.push(val);
		}

		void push(T&& val) {
			list.push(std::move(val));
		}
		void push(const stn::array<T>& val) {
			list.append(val);
		}
	private:
		stn::array<T> list;

	};

}
