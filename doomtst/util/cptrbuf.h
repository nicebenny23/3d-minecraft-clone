#include "dynamicarray.h"
#include "sharedptr.h"
#pragma once
using namespace Cptr;
namespace cptrbuffer {
    template<class T>
    struct cptrbuf
    {
    private:
        bool cangrow;
    public:
        int allocatedamt;
        array<cptr<T>> list;
        int size;
        cptrbuf() {

            allocatedamt = 0;
            size = 0;

        }
        // Default constructor

        int getfreeindex() {
            if (size == allocatedamt)
            {
                Assert("memory buffer filled");
            }
            int randomval;
            T* valifexist = nullptr;
            do {
                randomval = randomint(size);
                valifexist = list[randomval];
            } while (valifexist != nullptr);
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
        cptr<T>& operator[](int index)
        {
            return list[index];
        }

        // Instantiate the pool with a given size
        void instantiate(int newSize, bool shouldgrow = false)
        {
            if (size > 0) {
                clearscreen();
            }

            size = newSize;
            allocatedamt = 0;
            list = array<cptr<T>>(size, true);
            for (int i = 0; i < size; ++i)
            {
                list[i] = nullptr;
            }
        }

        // Append an element to the pool
        void append(cptr<T> elem)
        {



            int newindice = getfreeindex();
            list[newindice] = elem;
            elem->index = newindice;
            allocatedamt++;
        }



        // Remove an element by pointer
        void remove(cptr<T> elem)
        {


            list[elem->index]= nullptr;
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

}