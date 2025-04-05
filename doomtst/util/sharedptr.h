#pragma once
#include "../debugger/debug.h"
namespace sharedptr {
    template <typename T>
    class shared {
    private:
        T* ptr;                // Raw pointer to the managed object
        unsigned int* count;   // Reference count

    public:
      
        shared()
        {

            ptr = nullptr;
            count = nullptr;
        }

        // Constructor
        explicit shared(T* pointer)
        {
           
            ptr = pointer;
            count = nullptr;
            if (ptr != nullptr)
            {
                count = new unsigned int(1);
               
            }
        }
        // Copy constructor
        explicit shared(const shared<T>& other) {
            ptr = other.ptr;
            count = other.count;
            if (count != nullptr) {
                (*count)++;
            }
        }
        bool isvalid() {

            return (count && ptr);
        }
        void free() {

            
            if (isvalid())
            {

                (*count)--;

                if (*count == 0)
                {
                    destroy();
                }


                ptr = nullptr;
                count = nullptr;
            }
        }
        // Move constructor
      

        // Destructor


        shared& operator=(const shared<T>& other) {
            if (this != &other) {
                
                free();
                ptr = other.ptr;
                count = other.count;

            }
            return *this;
        }
        
        // Template Assignment Operator for Derived Types
        template <typename U>
        shared& operator=(const shared<U>& other) {
            static_assert(std::is_base_of<T, U>::value, "U must be derived from T");
            ptr= other.ptr;
            count = other.count;
            return *this;
        }

      
        bool operator==(T* other) {
            return ptr == other;
        }
        bool operator!=(T* other) {
            return ptr != other;
        }
        // Release the current object
        void destroy() {
            if (!isvalid())
            {
                Assert( "cant destory nullptr");
            }
            if (*count != 0) {
                Assert("element still exists,cant be deleted");
            }
                delete ptr;
                delete count;
            
           
            ptr = nullptr;
            count = nullptr;
        }


        // Get the raw pointer
        T* get() const {

            return ptr;
        }

        // Dereference operator
        T& operator*() const {
            if (ptr == nullptr)
            {
                Assert("cant derefrence nullptr");
            }

            return *ptr;
        }

        // Arrow operator
        T* operator->() const {
            if (ptr == nullptr)
            {
                Assert ("cant derefrence nullptr");
            }
            return ptr;
        }


        // Get the reference count
      
    };

}

namespace Cptr {

    template <typename T>
    struct cptr {
        cptr() {
            
            exists = sharedptr::shared<bool>(nullptr);
            
            pntr = nullptr;
        }
        sharedptr::shared<bool> exists;
    private:
        T* pntr;
        //unsafe function
        bool doesexist()
        {
            if (!exists.isvalid())
            {
                return false;
            }
            return *exists;

        }
        //unsafe function
        void updatestate() {


            if (!doesexist())
            {
                pntr = nullptr;

            }
        }

    public:
        explicit cptr(T* p)
        {
            pntr = p;
            if (pntr != nullptr)
            {

                exists = sharedptr::shared< bool>(new bool(true));
            }
            else {
                exists = sharedptr::shared<bool>(nullptr);
            }
        }

        // Template Assignment Operator for Derived Types
        template <typename U>
        void set(cptr<U>& other) {
            static_assert(std::is_base_of<T, U>::value, "U must be derived from T");
            exists = other.exists;
            pntr = other.ptr();
        }
        //safe
        cptr(const cptr<T>& other) {
            exists = other.exists;
            pntr = other.pntr;
        }



        //safe
        T*& ptr() {
            updatestate();
            return pntr;
        };

        //safe
        void  free() {
            updatestate();
            if (pntr == nullptr)
            {
                Assert("cant delete a nullptr");
            }
            //no need to check if exists as if it does not then the pointer becomes nullptr;
            else {
                delete pntr;
                pntr = nullptr;
             
                *exists = false;
                exists.free();
                
            }
        }


        T*& operator->() {

            return ptr();

        }
        //safe
        T& operator*() {
            return *ptr();
        }

        //safe
        bool operator==(T* other) {

            return ptr() == other;
        }
        //safe
        bool operator==(cptr<T>& other) {

            return ptr() == other.ptr();
        }
        bool operator!=(T* other) {

            return !(*this == other);
        }
        //safe
        bool operator!=(cptr<T>& other) {

            return  !(*this == other);
        }

    
    };

}