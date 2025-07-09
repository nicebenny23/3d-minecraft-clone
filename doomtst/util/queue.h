#pragma once
#include <stdexcept>
#include "deque.h"

namespace queuename {

    template<typename T>
    struct queue {
        Cont::deque<T> data;

        void push(const T& val) {
            data.push_back(val);
        }

        T pop() {
            if (empty()) throw std::out_of_range("Queue is empty");
           return data.pop_front();
        }

        T& front() {
            if (empty()) throw std::out_of_range("Queue is empty");
            return data.list[data.front];
        }

        const T& front() const {
            if (empty()) throw std::out_of_range("Queue is empty");
            return data.list[data.front];
        }

        T& back() {
            if (empty()) throw std::out_of_range("Queue is empty");
            return data.list[data.back];
        }

        const T& back() const {
            if (empty()) throw std::out_of_range("Queue is empty");
            return data.list[data.back];
        }

        bool empty() const {
            return data.empty();
        }

        size_t size() const {
            return data.length;
        }

        void clear() {
            data.destroy();
        }
    };

} // namespace queuename
