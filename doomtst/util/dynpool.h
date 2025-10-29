#pragma once
#include <stdexcept>
#include <cstddef>
#include <type_traits>
#include "dynamicarray.h"
#include "layout.h"
namespace dynPool {

    struct blockInfo {
        bool free;

        blockInfo() : free(true) {};
    };

    // A memory pool allocator for fixed-size blocks, properly aligned
    template<typename T>
    class flux {
    public:
        flux() {
            constexpr size_t infoSize = sizeof(blockInfo);
            // Align blockInfo size up to alignment of T
            size_t paddedInfoSize =stn::memory::layout_of<T>.align_up(infoSize);
            BlockSize = paddedInfoSize+ sizeof(T);
            // Store padded offset for easy access
            DataOffset = paddedInfoSize;
        }

        // For derived (variable element size), keep similar approach
        flux(stn::memory::layout layout) {
            size_t paddedInfoSize =layout.align_up(sizeof(blockInfo));
            if (!layout.fits(stn::memory::layout_of<T>)) {
                throw std::invalid_argument("Element size must be at least sizeof(T)");
            }
            BlockSize = paddedInfoSize + layout.size;
            DataOffset = paddedInfoSize;
        }

        // Move constructor
        flux(flux&& other) 
            : BlockSize(other.BlockSize),
            DataOffset(other.DataOffset),
            freeList(std::move(other.freeList)),
            pools(std::move(other.pools))
        {
            // leave other in a safe state
            other.BlockSize = 0;
            other.DataOffset = 0;
        }

        // Move assignment
        flux& operator=(flux&& other){
            if (this != &other) {
                destroy();
                // steal resources
                BlockSize = other.BlockSize;
                DataOffset = other.DataOffset;
                freeList = std::move(other.freeList);
                pools = std::move(other.pools);
            }
            return *this;
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

        ~flux() {
            destroy();
        }

    private:
        void destroy() {
            for (auto pool : pools) {
                if (pool == nullptr)
                {
                    throw std::logic_error("Postmortem Error:Pool was not allocated");
                }
                delete[] pool;
            }
            pools.clear();
            freeList.clear();

        }
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
            size_t poolnum = pools.length();
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

        stn::array<blockInfo*> freeList;
        stn::array<char*> pools;
    };

}
