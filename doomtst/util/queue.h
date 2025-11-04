#pragma once
#include <stdexcept>
#include "deque.h"

namespace stn {

    template<typename T>
    struct queue {

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

        T& front() {
            if (empty())
            {
                stn::throw_out_of_range("cannot get the front element of an empty queue");
            }
            return data.list[data.front];
        }

        const T& front() const {
            if (empty())
            {
                stn::throw_out_of_range("cannot get the front element of an empty queue");
            }
            return data.list[data.front];
        }

        T& back() {
            if (empty())
            {
                stn::throw_out_of_range("cannot get the back element of an empty queue");
            }
            return data.list[data.back];
        }

        const T& back() const {
            if (empty())
            {
                stn::throw_out_of_range("cannot get the back element of an empty queue");
            }
            return data.list[data.back];
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
    private:
        stn::deque<T> data;
    };

} // namespace Cont
