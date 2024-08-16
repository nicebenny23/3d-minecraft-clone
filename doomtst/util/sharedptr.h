#pragma once
#include "debug.h"
namespace sharedptr {
    template <typename T>
    class shared {
    private:
        T* ptr;                // Raw pointer to the managed object
        unsigned int* count;   // Reference count

    public:
        // Constructor
        explicit shared(T* p)
        {

            ptr = p;
            count = nullptr;
            if (ptr != nullptr)
            {
                count = new unsigned int(1);
                *count = 1;
            }
        }
         shared()
        {

            ptr = nullptr;
            count = nullptr;
        }
        // Copy constructor
        explicit shared(const shared<T>& other) {
            ptr = other.ptr;
            count = other.count;
            if (count != nullptr) {
                (*count)++;
            }
        }
        void free() {

            if (count == nullptr)
            {
                return;
            }
            if (ptr == nullptr)
            {
                return;
            }
            {

                (*count)--;

                if (*count == 0)
                {
                    destroy();
                }

            }
            ptr = nullptr;
            count = nullptr;
        }
        // Move constructor
        shared(shared<T>&& other) noexcept
            : ptr(other.ptr), count(other.count) {
            other.ptr = nullptr;
            other.count = nullptr;
        }

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
            reset();
            ptr= other.ptr;
            return *this;
        }

        // Move assignment operator
        shared<T>& operator=(shared<T>&& other) noexcept {

            if (this != &other) {
                free();
                ptr = other.ptr;
                count = other.count;
                other.ptr = nullptr;
                other.count = nullptr;

            }
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
            if (ptr == nullptr)
            {
                Assert( "cant destory nullptr");
            }
            if (count == nullptr)
            {
                int l = 1;
                Assert("dant destroy null ptr");
            }

            if (*count == 0) {
                delete ptr;
                delete count;
            }
            else {

                Assert("cant free null elem");
            }
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

        // Check if the pointer is unique
        bool unique() const {
            return count && *count == 1;
        }

        // Get the reference count
        unsigned int  usecount() const {
            return count ? *count : 0;
        }
    };

}

namespace Cptr {

    template <typename T>
    struct cptr {
        cptr() {
            
            exists = sharedptr::shared<bool>();
            
            pntr = nullptr;
        }
        sharedptr::shared<bool> exists;
    private:
        T* pntr;
        //unsafe function
        bool doesexist()
        {
            if (exists != nullptr)
            {

                bool doesexist = *exists;
                return doesexist;
            }
            return false;
        }
        //unsafe function
        void updatestate() {


            if (!doesexist())
            {
                pntr = nullptr;

            }
        }

    public:

        //safe
        void  destroy() {
            updatestate();
            if (pntr == nullptr)
            {
                Assert("cant delete null ptrs");
            }
            else {
                delete pntr;
                pntr = nullptr;
                if (exists != nullptr)
                {
                    *exists = false;
                    exists.free();
                }
            }
        }


        //safe
        T*& ptr() {
            updatestate();
            return pntr;
        };


        //safe
        bool operator==(T* other) {

            return ptr() == other;
        }
        //safe
        bool operator==(cptr<T>& other) {

            return ptr() == other.ptr();
        }
        //safe
        T*& operator->()  {

            return ptr();

        }
        //safe
        T& operator*() {
            return *ptr();
        }
        //safe
        explicit cptr(T* p)
        {
            exists = sharedptr::shared< bool>(new bool);
            *exists = true;
            pntr = p;

        }

        // Template Assignment Operator for Derived Types
        template <typename U>
        void cset(cptr<U>& other) {
            static_assert(std::is_base_of<T, U>::value, "U must be derived from T");
            exists = other.exists;
            pntr= other.ptr();
        }
        //safe
        cptr(const cptr<T>& other) {
            exists = other.exists;
            pntr = other.pntr;
        }

    };

}