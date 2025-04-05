#include "dynamicmempool.h"
#include <stdexcept>
#include <cstring> // For std::memset

namespace allocutil {
    poolqueue::poolqueue() : start(nullptr), end(nullptr), length(0) {}

    bool poolqueue::empty() {
        return length == 0;
    }

    blockmetadata* poolqueue::pop() {
        if (empty()) {
            throw std::runtime_error("Cannot pop from an empty queue");
        }
        blockmetadata* node = start;
        start = start->next;
        length--;
        if (empty()) {
            end = nullptr;
        }
        return node;
    }

    void poolqueue::push(blockmetadata* node) {
        if (empty()) {
            start = node;
            end = node;
        }
        else {

            end->next = node;
            end = node;
        }
        length++;
    }
}
using namespace allocutil;
namespace dynamicmempool {
    dynamicpool::dynamicpool(size_t blocksize, size_t poolsize)
    {
        blockSize = blocksize;
        poolSize = poolsize;
        totalblocksize = (blockSize + sizeof(blockmetadata));
        allocatenewpool();
    }

    dynamicpool::dynamicpool()
    {
        blockSize = 0;
        poolSize = 0;
        totalblocksize = 0;

    }



    void* dynamicpool::alloc() {
        void* block = findFreeBlock();
        if (block == nullptr) {
            throw std::bad_alloc(); // No free blocks available
        }
        return block;
    }

    void dynamicpool::free(void* ptr) {

        blockmetadata* node = GetMetaData(ptr);
        if (node->free)
        {
            throw std::logic_error("pointer already freed");
        
        }
        node->free = true;
        freeBlocks.push(node);
    }

    void dynamicpool::allocatenewpool() {
        char* newPool = new char[totalblocksize * poolSize];
        poollist.append(newPool);

        for (size_t i = 0; i < poolSize; ++i) {
            void* block = nthblock(newPool, i);
            blockmetadata* node = GetMetaData(block);
            node->free = true;
            node->next = nullptr;
            freeBlocks.push(node);
        }
    }

    blockmetadata* dynamicpool::GetMetaData(void* ptr) {
        return reinterpret_cast<blockmetadata*>(reinterpret_cast<char*>(ptr) + blockSize);
    }

    void* dynamicpool::getblockfrometadata(blockmetadata* metadata)
    {
        return reinterpret_cast<char*>(metadata) - blockSize;
    }

    void* dynamicpool::nthblock(void* base, int index) {
        return reinterpret_cast<char*>(base) + totalblocksize * index;
    }

    void* dynamicpool::operator[](int index)
    {
        if (index < 0|| poolSize * poollist.length <= index)
        {
            throw std::invalid_argument(" attempted to acess invalid pool");
        }
        int floorind = floor(index / poolSize);

        int modind = index - floorind * poolSize;

        return nthblock(poollist[floorind], modind);
    }
    //destroys all allocated memory
    void dynamicpool::destroy()
    {
        for (int i = 0; i < poollist.length; i++)
        {
            delete[]  poollist[i];
        }
        poollist.destroy();
        freeBlocks.length = 0;
        freeBlocks.start =new blockmetadata();
        freeBlocks.end= new blockmetadata();
    }

    void* dynamicpool::findFreeBlock() {
        if (freeBlocks.empty()) {
            allocatenewpool();
        }
        if (!freeBlocks.empty()) {
            blockmetadata* node = freeBlocks.pop();
            node->free = false;
            node->next = nullptr;
            return getblockfrometadata(node);

        }
        return nullptr;
    }
}