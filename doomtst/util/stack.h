#pragma once
#include "dynamicarray.h"

namespace stn {

	template<typename T>
	struct stack {
		stack() = default;

		bool empty() const {
			return list.empty();
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
	private:
		stn::array<T> list;

	};

}
