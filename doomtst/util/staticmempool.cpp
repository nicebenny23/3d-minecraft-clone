#include "staticmempool.h"

using namespace allocutil;
namespace staticmempool {
    staticpool::staticpool(size_t blocksize, size_t poolsize)
    {
        blockSize = blocksize;
        poolSize = poolsize;
        totalblocksize = (blockSize + sizeof(blockmetadata));
        allocatepool();
    }



    void* staticpool::allocate() {
        void* block = findFreeBlock();
        if (block == nullptr) {
            throw std::bad_alloc(); // No free blocks available
        }
        return block;
    }

    void staticpool::free(void* ptr) {

        blockmetadata* node = getblockmetadata(ptr);
        if (node->free)
        {
            Assert("cant free an already freed pointer");
        }
        node->free = true;
        freeBlocks.push(node);
    }

    void staticpool::allocatepool() {
        pool = new char[totalblocksize * poolSize];

        for (size_t i = 0; i < poolSize; i++) {
            void* block = nthblock(pool, i);
            blockmetadata* node = getblockmetadata(block);
            freeBlocks.push(node);
        }
    }

    blockmetadata* staticpool::getblockmetadata(void* ptr) {
        return reinterpret_cast<blockmetadata*>(reinterpret_cast<char*>(ptr) + blockSize);
    }

    void* staticpool::getblockfrometadata(blockmetadata* metadata)
    {
        return reinterpret_cast<char*>(metadata) - blockSize;
    }

    void* staticpool::nthblock(void* base, int index) {
        return reinterpret_cast<char*>(base) + totalblocksize * index;
    }

    void* staticpool::operator[](int index)
    {
        if (index < 0)
        {
            Assert("index less than zero");
        }
        if (poolSize <= index)
        {
            Assert("attempted to acess invalid pool");
        }
        return nthblock(pool, index);
    }

    void* staticpool::findFreeBlock() {
        if (freeBlocks.empty()) {
            Assert("pool overflow");
        }
        if (!freeBlocks.empty()) {
            blockmetadata* node = freeBlocks.pop();
            node->free = false;
            return getblockfrometadata(node);
        }
        return nullptr;
    }
}