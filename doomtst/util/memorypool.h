#include "dynamicarray.h"
#include <algorithm>
#include "random.h"
#include <stdexcept>
using namespace Cont;
#pragma once
//not actually a memory pool it is a randomized pool
template<class T>
struct ptrmempool
{
private:
    bool cangrow;
public:
    int allocatedamt;
    array<T*> list;
    int size;
    ptrmempool() {

        allocatedamt = 0;
        size = 0;
        
    }
    // Default constructor
  
    int getfreeindex() {
        if (size==allocatedamt)
        {
            Assert("memory buffer filled");
        }
        int randomval;
        T* valifexist=nullptr;
        do {
            randomval = randomint(size);
            valifexist = list[randomval];        } while (valifexist != nullptr);
        return randomval;
    }
    // Calculate the density of the pool
    float density() const
    {
        if (size == 0) {
            return 1;
        }
        return static_cast<float>(allocatedamt) / size;
    }

    // Access element by index
    T* operator[](int index)
    {
            return list[index];
    }

    // Instantiate the pool with a given size
    void instantiate(int newSize,bool shouldgrow=false,bool instantiateelements=false)
    {
        if (size > 0) {
            clearscreen();
        }

        size = newSize;
        allocatedamt = 0;
        list = array<T*>(size);
        for (int i = 0; i < size; ++i)
        {
            list[i] = nullptr;
        }
    }

    // Append an element to the pool
    void push(T* elem)
    {

       
         
            int newindice = getfreeindex();
            list[newindice] = elem;
            elem->index = newindice;
            allocatedamt++;
    }

    

    // Remove an element by pointer
    void remove(T* elem)
    {


        list[elem->index] = nullptr;
        elem->index = -1;
        allocatedamt--;
    }

  
    // Clear all elements and deallocate memory
    void clearscreen()
    {
        for (int i = 0; i < size; ++i)
        {
            list[i] = nullptr;
        }
      
        allocatedamt = 0;
        size = 0;
    }

 
};