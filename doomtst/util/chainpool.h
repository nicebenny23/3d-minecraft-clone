#pragma once
//essientially a linked list with very fast allocation times,unfortonatly not <T> supported yet but i might add support if i need a linked list
#include <iterator>
#include "dynamicarray.h"
using namespace dynamicarray;

#include <cstddef>
namespace chainutil {
    struct blockmetadata {
    public:
        bool free;
        blockmetadata* next;
        blockmetadata* prev;
        blockmetadata() {
            free = true;
            next = nullptr;
            prev = nullptr;
        }

    };
    struct metalist
    {
        blockmetadata* start;
        blockmetadata* end;
        int length;
        metalist();
        bool empty();
        blockmetadata* pop();
        void remove(blockmetadata* block);
        void push(blockmetadata* node);
        void reset();
    private:
        void shrink();
    };
    //stores value and metadata to the side
};

namespace chainpool {
    using namespace chainutil;
    //no destructer as destructers can make it more difficult in game engines
    template < typename T>
    class chainedpool {
    public:



        class Iterator {


        private:
            chainedpool* owner;
            blockmetadata* ptr;
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = char*;
            using difference_type = std::ptrdiff_t;
            using pointer = char**;
            using reference = char*&;

            // Constructor initializes the iterator with a pointer.
            Iterator(chainedpool* ownerpool, blockmetadata* p) {

                owner = ownerpool;
                ptr = p;
            }

            T& operator*() {
                return *owner->GetElement(ptr);
            }


            Iterator& operator++() {
                ptr = ptr->next;
                return *this;
            }
            Iterator& operator--() {
                ptr = ptr->prev;
                return  *this;
            }


            bool operator!=(const Iterator& other) const {
                return ptr != other.ptr;
            }
        };

        Iterator begin() {
            return Iterator(this, usedBlocks.start);
        }

        Iterator end() {
            return Iterator(this, nullptr);
        }

        size_t get_size() const {
            return usedBlocks.length;
        }






        chainedpool(size_t poolsize);
        chainedpool(size_t ElemSize, size_t poolsize);
        explicit chainedpool();
        // Allocate memory for an object
        T* alloc();

        // Deallocate memory previously allocated
        void free(T* ptr);
        int size() {

            return poolSize;
        }
        int capacity() {

            return poolSize * poollist.length;
        }
        //iterayes over all used blocks


    private:

        blockmetadata* GetMetaData(T* ptr);
        T* GetElement(blockmetadata* metadata);
        T* GetBlockInPool(int pool, int ind);
        T* operator[](int index);//gets nth memeory cell



        void CreatePool();


        void destroy();

        size_t BlockSize;//size of each block and its assosiated metadata
        size_t ElemSize; // Size of each block without metadata
        size_t poolSize;  // Number of blocks in the pool
        metalist usedBlocks; // Array to track free blocks
        metalist freeBlocks; // Array to track free blocks
        array<char*> poollist;   // Pointer to the memory pool

        T* findFreeBlock(); // Find a free block index


    };

}







using namespace chainutil;

namespace  chainpool {

    template<typename T>
    //ONLY USE WHEN T IS NOT DERIVABLE
    chainedpool<T>::chainedpool(size_t poolsize)
    {

        ElemSize = sizeof(T);
        poolSize = poolsize;
        BlockSize = (ElemSize + sizeof(blockmetadata));
        CreatePool();
    }
    template < typename T>
    chainedpool<T>::chainedpool(size_t ElementSize, size_t poolsize)
    {
        ElemSize = ElementSize;
        poolSize = poolsize;
        BlockSize = (ElemSize + sizeof(blockmetadata));
        CreatePool();
    }
    template < typename T>
    chainedpool<T>::chainedpool()
    {
        ElemSize = 0;
        poolSize = 0;
        BlockSize = 0;

    }


    template < typename T>
    T* chainedpool<T>::alloc() {
        T* block = findFreeBlock();
        if (block == nullptr) {
            throw std::bad_alloc(); // No free blocks available
        }
        return block;
    }
    template < typename T>
    void chainedpool<T>::free(T* ptr) {

        blockmetadata* node = GetMetaData(ptr);
        if (node->free)
        {
            throw std::logic_error("Error:Cannot free pointer which has already been freed");

        }
        usedBlocks.remove(node);
        node->free = true;
        freeBlocks.push(node);

    }
    template < typename T>
    void chainedpool<T>::CreatePool() {
        char* newPool = new char[BlockSize * poolSize];
        poollist.append(newPool);
        int poolnum = poollist.length - 1;
        for (size_t i = 0; i < poolSize; ++i) {
            T* block = GetBlockInPool(poolnum, i);
            blockmetadata* node = GetMetaData(block);
            freeBlocks.push(node);
        }
    }
    template < typename T>
    blockmetadata* chainedpool<T>::GetMetaData(T* element) {
        return reinterpret_cast<blockmetadata*>(reinterpret_cast<char*>(element) + ElemSize);
    }
    template < typename T>
    T* chainedpool<T>::GetElement(blockmetadata* metadata)
    {
        return reinterpret_cast<T*>(reinterpret_cast<char*>(metadata) - ElemSize);
    }
    template < typename T>
    T* chainedpool<T>::GetBlockInPool(int pool, int BlockIndex) {
        return reinterpret_cast<T*>(poollist[pool]+ BlockSize * BlockIndex);
    }
    template < typename T>
    T* chainedpool<T>::operator[](int index)
    {
        if (index < 0 || poolSize * poollist.length <= index)
        {
            throw std::invalid_argument("Attempted to acess invalid pool");
        }
        int pool = floor(index / poolSize);

        int poolIndex = index - pool * poolSize;

        return GetBlockInPool(pool, poolIndex);
    }
    template < typename T>
    void chainedpool<T>::destroy()
    {

        for (int i = 0; i < poollist.length; i++)
        {
            delete[]  poollist[i];
        }
        poollist.destroy();
        freeBlocks.reset();
        usedBlocks.reset();

    }
    template < typename T>
    T* chainedpool<T>::findFreeBlock() {
        if (freeBlocks.empty()) {
            CreatePool();
        }
        if (!freeBlocks.empty()) {
            blockmetadata* node = freeBlocks.pop();
            node->free = false;
            usedBlocks.push(node);
            return GetElement(node);

        }
        return nullptr;
    }
}
#pragma once
