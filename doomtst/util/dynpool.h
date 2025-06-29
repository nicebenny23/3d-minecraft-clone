#pragma once
#include "dynamicarray.h"
#include <stdexcept>

namespace DynPool {

    struct blockInfo {
        bool free;

        blockInfo() : free(true) {};
    };

    // A memory pool allocator for fast allocation of fixed-size blocks
    // Block layout: [BlockInfo][T]. Pools grow exponentially (2^pool_index).
    template<typename T>
    class flux {
    public:
        // For Non Derived
        flux() {
            BlockSize = (sizeof(T) + sizeof(blockInfo));
        }

        // For Derived
        flux(size_t ElementSize) {
            if (BlockSize < sizeof(T)) {
                throw std::invalid_argument("Element size must be at least sizeof(T)");
            }
            BlockSize = (ElementSize + sizeof(blockInfo));
        }

        T* alloc() {
            if (freeList.empty()) {
                CreatePool();
            }

            blockInfo* node = freeList.pop();
            node->free = false;
            return GetElement(node);
        }

        void free(T* ptr) {

            blockInfo* node = GetMetaData(ptr);
            if (node->free)
            {
                throw std::logic_error("Error:Cannot free pointer which has already been freed");
            }
            PushToFreeList(node);

        }

        void destroy()
        {
            for (auto pool : pools)
            {
                delete[] pool;
            }
            pools.destroy();
            freeList.destroy();
        }

    private:

        blockInfo* GetMetaData(T* element) {
            return reinterpret_cast<blockInfo*>(reinterpret_cast<char*>(element) - sizeof(blockInfo));
        }

        T* GetElement(blockInfo* metadata)
        {
            return reinterpret_cast<T*>(reinterpret_cast<char*>(metadata) + sizeof(blockInfo));
        }
        // Pushes the element to the FreeList
        void PushToFreeList(blockInfo* node)
        {
            node->free = true;
            freeList.push(node);
        }

        void CreatePool() {

            size_t poolnum = pools.length;
            size_t newPoolLength = 1 << poolnum;
            char* newPool = new char[BlockSize * newPoolLength];
            pools.push(newPool);
            for (size_t i = 0; i < newPoolLength; i++) {
                blockInfo* node = reinterpret_cast<blockInfo*>(i * BlockSize + newPool);
                PushToFreeList(node);
            }
        }

        size_t BlockSize; 
        Cont::array<blockInfo*, false> freeList;
        Cont::array<char*> pools;

    };
}
