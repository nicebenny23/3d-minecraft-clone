#pragma once
#include"dynamicmempool.h"


namespace staticmempool {
    class staticpool {
    public:
        // Constructor initializes the pool with a specific block size and pool size
        staticpool(size_t blocksize, size_t poolsize);
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
        void allocatepool();
        allocutil::blockmetadata* getblockmetadata(void* ptr);
        void* getblockfrometadata(allocutil::blockmetadata* metadata);
        void* nthblock(void* base, int ind);
        void* operator[](int index);
        char* pool;       // Pointer to the memory pool
        allocutil::poolqueue freeBlocks; // Array to track free blocks
        void* findFreeBlock(); // Find a free block index
    };
}