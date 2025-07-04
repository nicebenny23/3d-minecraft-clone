#pragma once
#include "dynamicarray.h"
#include <stdexcept>
#include <atomic>

namespace type_id {

    inline std::atomic<size_t> global_type_counter = 0;
    template<typename T>
    size_t get_typeid() {
        static const size_t id = global_type_counter.fetch_add(1, std::memory_order_relaxed);
        return id;
    }
    
    struct dense_type_system {
        size_t type_index;

       Cont::array<size_t> sparse_map;
        dense_type_system()
        {
            type_index = 1;
        }


        template<typename T>
        size_t get() {
            size_t id= get_typeid<T>();
            size_t &dense_id= sparse_map[id];
            if (dense_id==0)
            {
                dense_id = type_index;
                type_index++;
            }
            return dense_id;
        }
        template<typename T>
        bool contains() const{
            size_t id = get_typeid<T>();
            size_t dense_id  = sparse_map[id];
            return (dense_id != 0);
        }
    };
}
