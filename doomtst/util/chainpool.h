
//essientially a linked list with very fast allocation times,unfortonatly not <T> supported yet but i might add support if i need a linked list
#include <iterator>
#include "dynamicarray.h"#include <cstddef>
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

            bool operator==(const Iterator& other) const {
                return ptr == other.ptr;
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




    public:

        chainedpool(size_t ElemSize);
        explicit chainedpool();
        // Allocate memory for an object
        T* alloc();

        // Deallocate memory previously allocated
        void free(T* ptr);

        //iterayes over all used blocks

        void destroy();

    private:

        blockmetadata* GetMetaData(T* ptr);
        T* GetElement(blockmetadata* metadata);

        blockmetadata* NthMetaData(char* pool, size_t n) {

            return reinterpret_cast<blockmetadata*>(n * BlockSize + pool + ElemSize);
        }

        void CreatePool();



        size_t BlockSize;//size of each block and its assosiated metadata
        size_t ElemSize; // Size of each block without metadata

        metalist usedBlocks; // Array to track free blocks
        metalist freeBlocks; // Array to track free blocks
        Cont::array<char*> poollist;   // Pointer to the memory pool

        T* findFreeBlock(); // Find a free block index


    };

}







using namespace chainutil;

namespace  chainpool {

    template<typename T>
    //ONLY USE WHEN T IS NOT DERIVABLE
    chainedpool<T>::chainedpool()
    {

        ElemSize = sizeof(T);

        BlockSize = (ElemSize + sizeof(blockmetadata));

    }
    template < typename T>
    chainedpool<T>::chainedpool(size_t ElementSize)
    {
        ElemSize = ElementSize;
        BlockSize = (ElemSize + sizeof(blockmetadata));

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

        int poolnum = poollist.length;
        size_t poolSize = 1 << poolnum;
        char* newPool = new char[BlockSize * poolSize];
        poollist.push(newPool);
        for (size_t i = 0; i < poolSize; i++) {
            blockmetadata* node = NthMetaData(newPool, i);
            new (node) blockmetadata{};
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
    //gets block Blockindex in pool pool

    //destructor not needed as these objects are always global
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