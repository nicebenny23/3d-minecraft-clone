#ifndef STATICMEMORYPOOL_H
#define STATICMEMORYPOOL_H
#include "dynamicarray.h"
using namespace dynamicarray;

#include "queue.h"
#include <cstddef>
namespace allocutil {
    struct blockmetadata {
    public:
        bool free;
        blockmetadata* next;
        blockmetadata() {
            free = true;
            next = nullptr;
        }
    };
    struct poolqueue
    {
        blockmetadata* start;
        blockmetadata* end;
        int length;
        poolqueue();
        bool empty();
        blockmetadata* pop();
        inline void push(blockmetadata* node);
    };
    //stores value and metadata to the side
};

namespace dynamicmempool {
    class dynamicpool {
    public:
        // Constructor initializes the pool with a specific block size and pool size
        dynamicpool(size_t blocksize, size_t poolsize);
       explicit dynamicpool();
        // Allocate memory for an object
        void* allocate();


        // Deallocate memory previously allocated
        void free(void* ptr);
        int size() {

            return poolSize;
        }
    private:
        size_t totalblocksize;
        size_t blockSize; // Size of each block
        size_t poolSize;  // Number of blocks in the pool
        void allocatenewpool();
        allocutil::blockmetadata* getblockmetadata(void* ptr);
        void* getblockfrometadata(allocutil::blockmetadata* metadata);
        void* nthblock(void* base, int ind);
        array<char*> poollist;   // Pointer to the memory pool
        void* operator[](int index);
        allocutil::poolqueue freeBlocks; // Array to track free blocks
        void* findFreeBlock(); // Find a free block index
    };

}
#endif // STATICMEMORYPOOL_H