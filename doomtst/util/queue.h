#pragma once
#include <stdexcept>
#include "deque.h"

namespace stn {

    template<typename T>
    struct queue {
        template<typename ...Args>
        void emplace(Args&&... args) {
            data.emplace_back(std::forward<Args>(args)...);
        }

		template<std::ranges::forward_range R>
			requires std::same_as<std::ranges::range_value_t<R>, T>
		void push_many(R&& range) {
			data.push_back(std::forward<R>(range));
		}
        void push(const T& val) {
            data.push_back(val);
        }
        void drop_many(size_t count) {
            data.drop_back_many(count);
        }
        void drop() {
            return data.drop_back();
        }
        T pop() {
            if (empty())
            {
                stn::throw_out_of_range("cannot pop from from an empty queue");
            }
            return data.pop_front();
        }
        bool empty() const {
            return data.empty();
        }

        size_t length() const {
            return data.length();
        }

        void clear() {
            data.clear();
        }
        using iterator = stn::deque<T>::iterator;
        iterator begin() {
            return data.begin();
        }
        iterator end() {
            return data.end();
        }
    private:
        stn::deque<T> data;
    };

} // namespace Cont
