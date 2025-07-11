#pragma once
#include <stdexcept>
#include <cstddef>
#include <type_traits>
#include "dynamicarray.h"

namespace dynPool {

    struct blockInfo {
        bool free;

        blockInfo() : free(true) {};
    };

    // Helper: Align `size` up to nearest multiple of `alignment`
    constexpr size_t align_up(size_t size, size_t alignment) {
        return (size + alignment - 1) & ~(alignment - 1);
    }

    // A memory pool allocator for fixed-size blocks, properly aligned
    template<typename T>
    class flux {
    public:
        flux() {
            constexpr size_t align = alignof(T);
            constexpr size_t infoSize = sizeof(blockInfo);
            // Align blockInfo size up to alignment of T
            size_t paddedInfoSize = align_up(infoSize, align);
            BlockSize = paddedInfoSize + sizeof(T);
            // Store padded offset for easy access
            DataOffset = paddedInfoSize;
        }

        // For derived (variable element size), keep similar approach
        flux(size_t ElementSize,size_t Alignment) {
             size_t align = Alignment;
            size_t paddedInfoSize = align_up(sizeof(blockInfo), align);
            if (ElementSize < sizeof(T)) {
                throw std::invalid_argument("Element size must be at least sizeof(T)");
            }
            BlockSize = paddedInfoSize + ElementSize;
            DataOffset = paddedInfoSize;
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
            if (node->free) {
                throw std::logic_error("Error: Cannot free pointer which has already been freed");
            }
            PushToFreeList(node);
        }

        void destroy() {
            for (auto pool : pools) {
                if (pool==nullptr)
                {
                    throw std::logic_error("Postmortem Error:Pool was not allocated");
                }
                delete[] pool;
            }
            pools.destroy();
            freeList.destroy();
        }

    private:
        // Get pointer to blockInfo from data pointer
        blockInfo* GetMetaData(T* element) {
            return reinterpret_cast<blockInfo*>(
                reinterpret_cast<char*>(element) - DataOffset
                );
        }

        // Get pointer to data from blockInfo pointer
        T* GetElement(blockInfo* metadata) {
            return reinterpret_cast<T*>(
                reinterpret_cast<char*>(metadata) + DataOffset
                );
        }

        void PushToFreeList(blockInfo* node) {
            node->free = true;
            freeList.push(node);
        }

        void CreatePool() {
            size_t poolnum = pools.length;
            size_t newPoolLength = 1 << poolnum;
            char* newPool = new char[BlockSize * newPoolLength];
            pools.push(newPool);

            for (size_t i = 0; i < newPoolLength; i++) {
                blockInfo* node = reinterpret_cast<blockInfo*>(newPool + i * BlockSize);
                PushToFreeList(node);
            }
        }

        size_t BlockSize;  // Total size per block (metadata + aligned payload)
        size_t DataOffset; // Offset from blockInfo to T*, aligned properly

        Cont::array<blockInfo*, false> freeList;
        Cont::array<char*> pools;
    };

}
