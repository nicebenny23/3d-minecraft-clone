#pragma once
#include "dynamicarray.h"

namespace stn {

    template<typename T>
    struct stack {
    private:
        stn::array<T> list;

    public:
        T pop();
        T& peek();
        const T& peek() const;
        void push(const T& val);
        void push( T&& val);
        stack();
        bool empty() const;
        void destroy();
        stack(const stack& other);
        stack& operator=(const stack& other);
    };

    template<typename T>
    stack<T>::stack() {}

    template<typename T>
    T& stack<T>::peek() {
        if (!empty()) {
            return list[list.length - 1];
        }
        throw std::out_of_range("Error: Attempted to peek an empty stack.");
    }

    template<typename T>
    const T& stack<T>::peek() const {
        if (!empty()) {
            return list[list.length - 1];
        }
        throw std::out_of_range("Error: Attempted to peek an empty stack.");
    }

    template<typename T>
    T stack<T>::pop() {
        if (empty()) {
            throw std::out_of_range("Error: Attempted to pop from an empty stack.");
        }
        return list.pop();
    }

    template<typename T>
    void stack<T>::push(const T& val) {
        list.push(val);
    }
    template<typename T>
    void stack<T>::push( T&& val) {
        list.push(std::move(val));
    }

    template<typename T>
    bool stack<T>::empty() const {
        return list.length == 0;
    }

    template<typename T>
    void stack<T>::destroy() {
        list.destroy();
    }

    template<typename T>
    stack<T>& stack<T>::operator=(const stack<T>& other) {
        list = other.list;
        return *this;
    }

    template<typename T>
    stack<T>::stack(const stack& other) {
        list = other.list;
    }

}
