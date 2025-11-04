#pragma once
#include <stdexcept>
namespace sharedptr {
    template <typename T>
    class rc {
    private:
        T* ptr;                // Raw pointer to the managed object
        unsigned int* count;   // Reference count
        void incrementCount() {
            if (count) {
                (*count)++;
            }
        }
        
        void destroy() {
            delete ptr;
            delete count;
            ptr = nullptr;
            count = nullptr;
        }


    public:

        [[nodiscard]] bool isValid() const {

            return count!=nullptr;
        }
        void free() {
            if (count)
            {
                    (*count)--;
                    if (*count == 0) {
                        destroy();
                    }
            }

            ptr = nullptr;
            count = nullptr;

        }

        void clear(T* pointer=nullptr) {
            if (ptr == pointer) {
                return;
            }

           free();
            ptr = pointer;
            if (pointer != nullptr)
            {
                count = new unsigned int(1);
            }
        }
        // Move constructor







        rc(): ptr(nullptr),count(nullptr)
        {
        }
        ~rc() {

            free();
        }
        // Constructor
        explicit rc(T* pointer):ptr(pointer),count(nullptr)
        {
            if (ptr != nullptr)
            {
                count = new unsigned int(1);
               
            }
        }
        // Copy constructor
        rc(const rc<T>& other) : ptr(other.ptr), count(other.count)  {
            incrementCount();
        }
    
        // Template Assignment Operator for Derived Types
        template <typename U>
        rc& operator=(const rc<U>& other) {
            static_assert(std::is_base_of<T, U>::value, "U must be derived from T");
            if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
                free();
                ptr = other.ptr;
                count = other.count;
                incrementCount();
            }
            return *this;
        }


        rc& operator=(const rc<T>& other) {
            if (this != &other) {
                
                free();
                ptr = other.ptr;

                count = other.count;
                incrementCount();
            }
            return *this;
        }
        
        rc(rc<T>&& other) noexcept {
            ptr = std::exchange(other.ptr, nullptr);
            count = std::exchange(other.count, nullptr);
        }

        rc& operator=(rc<T>&& other) noexcept {
            if (this != &other) {
                free();
               ptr= std::exchange(other.ptr, nullptr);
               count = std::exchange(other.count, nullptr);
            }
            return *this;
        }
        // Get the reference count




      
   


        // Get the raw pointer
        T* get() const {

            return ptr;
        }
        T& operator*() const {
            if (!ptr) {
                throw std::runtime_error("Dereferencing null shared pointer");
            }
            return *ptr;
        }
        T* operator->() const {
            if (!ptr) {
                throw std::runtime_error("Dereferencing null shared pointer");
            }
            return ptr;
        }
        
  bool operator==(T* other) const {
            return ptr == other;
        }
 bool operator!=(T* other) const {
            return ptr != other;
        }
        // Release the current object
    
    };

}

namespace Cptr {

    template <typename T>
    struct cptr {
      
        sharedptr::rc<bool> exists;
    private:
        T* pntr;
        //unsafe function
       
        //unsafe function
        void updatestate() {


            if (!exists.isValid() || !*exists)
            {
                pntr = nullptr;
            }
        }

    public:
        cptr() = default;
        cptr(const cptr<T>& other) = default;


        explicit cptr(T* p)
        {
            pntr = p;
            if (pntr != nullptr)
            {

                exists = sharedptr::rc< bool>(new bool(true));
            }
            else {
                exists = sharedptr::rc<bool>(nullptr);
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
        bool valid() {
            updatestate();
            return (pntr != nullptr);
          

        }



        //safe
        T*& ptr() {
            updatestate();
            return pntr;
        };

        //safe
        void free() {
            updatestate();
            if (pntr == nullptr)
            {
                throw std::logic_error("cant delete a nullptr");
            }
            
                delete pntr;
                pntr = nullptr;
                if (exists.isValid())
                {

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
        bool operator==( T* other)  {

            return ptr() == other;
        }
        //safe
        bool operator==(cptr<T>& other){

            return ptr() == other.ptr();
        }
        bool operator!=(T* other)  {

            return (ptr() != other);
        }
        //safe
        bool operator!=(cptr<T>& other) {

            return  (ptr() != other.ptr());
        }

        cptr(cptr&& other) noexcept = default;
        cptr& operator=(cptr&& other) noexcept = default;
    };

}