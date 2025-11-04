#pragma once
#include <stdexcept>
#include <cstddef>
#include <type_traits>
#include "dynamicarray.h"
#include "layout.h"
#include <typeinfo>
#include "type_index.h"
#include <cassert>
namespace stn {
    template<class T>
    concept Complete = requires { sizeof(T); };
    template<typename A,typename B>
    struct alignas(8) dual_ptr {

        std::uintptr_t ptr;
        static constexpr uintptr_t tag_mask = 1;

        dual_ptr() {

            static_assert(!Complete<A> || alignof(A) >= 2,
                "A must have >= 2-byte alignment");
            static_assert(!Complete<B> || alignof(B) >= 2,
                "B must have >= 2-byte alignment");

            ptr = 0;
        }
 
        template<typename T>
            requires(std::is_same_v<T, A> || std::is_same_v<T, B>)
        inline constexpr void from(std::type_identity_t<T>* other_ptr) noexcept {

            if constexpr (std::is_same_v<T, A>) {
                ptr = reinterpret_cast<std::uintptr_t>(other_ptr);
            }
            else {
                ptr = reinterpret_cast<std::uintptr_t>(other_ptr) | tag_mask;
            }
        }
        // Type test
        template<typename T>
            requires(std::is_same_v<T, A> || std::is_same_v<T, B>)
        inline constexpr bool is() const noexcept {
            if constexpr (std::is_same_v<T, A>) {
                return (ptr & tag_mask) == 0;
            }
            else {
                return (ptr & tag_mask) != 0;
            }
        }

        // Access
        template<typename T>
            requires(std::is_same_v<T, A> || std::is_same_v<T, B>)
        inline T& as() {
            if constexpr (std::is_same_v<T, A>) {
                assert(is<T>() && "dual_ptr::as<A>() — type was not A");
                return *reinterpret_cast<A*>(ptr);
            }
            else {
                assert(is<T>() && "dual_ptr::as<B>() — type was not B");
                return *reinterpret_cast<B*>(ptr & ~tag_mask);
            }
        }

        template<typename T>
            requires(std::is_same_v<T, A> || std::is_same_v<T, B>)
        inline const T& as() const {
            if constexpr (std::is_same_v<T, A>) {
                assert(is<T>() && "dual_ptr::as<A>() — type was not A");
                return *reinterpret_cast<const A*>(ptr);
            }
            else {
                assert(is<T>() && "dual_ptr::as<B>() — type was not B");
                return *reinterpret_cast<const B*>(ptr & ~tag_mask);
            }
        }

    };

    template<typename T>
    struct flux_token;
    struct flux;
    // Metadata for each allocated block. 
    // - When free:  next points to the next free block.
    // - When used:  allocator_ptr == owning flux.
    struct alignas(8) block_info {
        //dont store as a union to prevent ub when checking if it occupied
    
        dual_ptr<block_info, flux> ptr;
    };
    //align of block by itsel
    constexpr size_t block_info_gap = stn::memory::align_with(sizeof(block_info), alignof(block_info));

    constexpr stn::memory::layout block_info_layout = stn::memory::layout(block_info_gap, alignof(block_info));

    // Behaves similarly to std::unique_ptr<T> but supports some dynamic type features at a faster speed/ also supports raii
    // returns memory back to the owning flux pool instead of deallocating it.
    template<typename T>
    struct flux_token {
        flux_token(const flux_token& other) = delete;
        flux_token operator=(const flux_token& other) = delete;
        flux_token() :ptr(nullptr) {};

        flux_token& operator=(flux_token&& other) noexcept {
            if (this != &other) {
                release();
                ptr = other.ptr;
                other.ptr = nullptr;
            }
            return *this;
        }
        flux_token(flux_token&& other) noexcept {
            ptr = other.ptr;
            other.ptr = nullptr;
        };
        explicit operator bool() const noexcept {
            return ptr != nullptr;
        }
        T& operator*() {
            if (!ptr)
            {
                throw std::runtime_error("cannot derefrence a null memory token");
            }
            return *ptr;
        }
        const T& operator*() const {
            if (!ptr)
            {
                throw std::runtime_error("cannot derefrence a null memory token");
            }
            return *ptr;
        }
        T* get_ptr() {
            return ptr;
        }
        const T* get_ptr() const {
            return ptr;
        }
        T& get_unchecked() {
            return *ptr;
        }
        const T& get_unchecked() const {
            return *ptr;
        }
        // Unchecked reinterpret casts to compatible types
        template<typename U>
        U* get_unchecked_ptr() {
            return reinterpret_cast<U*>(ptr);
        }

        template<typename U>
        const U* get_unchecked_ptr() const {
            return reinterpret_cast<U*>(ptr);
        }
        template<typename U>
        U& get_unchecked_as() {
            return *get_unchecked_ptr<U>();
        }

        template<typename U>
        const U& get_unchecked_as() const {
            return *get_unchecked_ptr<U>();
        }

        // Get type ID of the pointed object (throws if null)
        type_map::type_id id() const {
            if (ptr == nullptr)
            {
                throw std::logic_error("cannot get the type_id of a nullptr");
            }
            return id_unchecked();
        }

        // Get type ID of the pointed object (Ub if null)
        inline type_map::type_id id_unchecked() const
        {
            return get_flux().object_id;
        }

        //converts a memory_token from a derived class to a base class
        template<typename U>
        flux_token<U> abstract() requires std::derived_from<T, U> {
            flux_token<U> other(reinterpret_cast<U*>(ptr));
            ptr = nullptr;
            return other;
        };

        // downcasts a class from a base to a derived class
        template<typename U>
        flux_token<U> downcast() requires std::derived_from<U, T> {
            if (ptr)
            {
                if (id_unchecked() != type_map::type_id::make_type_id<U>()) {
                    stn::throw_range_exception("Cannot downcast flux_token of type {} to {}", get_flux().stored_type_info->name(), typeid(U).name());
                }
            }
            return downcast_unchecked<U>();
        };
        template<typename U>
        flux_token<U> downcast_unchecked() requires std::derived_from<U, T>
        {
            flux_token<U> other(reinterpret_cast<U*>(ptr));
            ptr = nullptr;
            return other;
        }
        inline void release()
        {
            if (ptr != nullptr)
            {
                get_flux().destroy_unchecked(ptr);
                ptr = nullptr;
            }
        }

        ~flux_token()
        {
            release();
        }

    private:

        // Get reference to owning flux allocator(since all memory tokens poin
        inline flux& get_flux() {
            return get_block_info().ptr.as<flux>();
        }
        inline const flux& get_flux() const {
            return get_block_info().ptr.as<flux>();
        }

        //since our memory layout is [[block_info,padding](block_info_gap),object...]
        // Retrieve block metadata for this token’s object which is alway in allocated state because it can only be created in emplace which is always in allocated state
        inline block_info& get_block_info() {
            return *reinterpret_cast<block_info*>(reinterpret_cast<std::byte*>(ptr) - block_info_gap);
        }
        inline const block_info& get_block_info() const {
            return *reinterpret_cast<block_info*>(reinterpret_cast<std::byte*>(ptr) - block_info_gap);
        }

        template <typename U>
        friend struct flux_token;
        flux_token(T* pointer) :ptr(pointer) {};
        friend struct flux;
        T* ptr;



    };
    // A type safe type erased memory pool allocator     
    struct flux {
        //constructs from a derived class
        template<typename T>
        flux(std::type_identity<T> identity) : object_id(type_map::type_id::make_type_id<T>()) {
            destructor = [](void* ptr) {
                ((T*)(ptr))->~T(); 
                };
            //we want the object to be accesabble without caring about the specific type so we treat block_info naivly as its own object with offset [[blockinfo],[object]]
            //we can just 
            stn::memory::layout object_layout = stn::memory::layout_of<T>;
            stn::memory::layout info_layout(block_info_gap, alignof(block_info));
            block_layout = stn::memory::join(object_layout, info_layout);
            stored_type_info = &typeid(T);
            top = nullptr;
        }
        flux(const flux& other) = delete;

        flux& operator=(const flux& other) = delete;
        // Move constructor
        flux(flux&& other) noexcept
            : block_layout(std::move(other.block_layout)),
            top(std::move(other.top)),
            pools(std::move(other.pools)),
            object_id(std::move(other.object_id)),
            destructor(std::move(other.destructor)),
            stored_type_info(std::move(other.stored_type_info))
        {
            reassign_allocator_ptrs();
            other.top = nullptr;
            other.stored_type_info = nullptr;
        }
        friend void swap(flux& a, flux& b) noexcept {
            using std::swap;
            swap(a.destructor, b.destructor);
            swap(a.object_id, b.object_id);
            swap(a.block_layout, b.block_layout);
            swap(a.pools, b.pools);
            swap(a.top, b.top);
            swap(a.stored_type_info, b.stored_type_info);
            a.reassign_allocator_ptrs();
            b.reassign_allocator_ptrs();
        }

        // Move assignment
        flux& operator=(flux&& other) noexcept {
            if (this != &other) {
                clear();
                swap(*this, other);
            }
            return *this;
        }

        ~flux() {
            clear();
        }
        //returns if their is any allocations in the pool
        size_t allocated_count() const {
            size_t count = 0;
            for (size_t i = 0; i < number_of_pools(); i++)
            {
                size_t length = size_of_pool(i);
                for (size_t j = 0; j < length; j++)
                {
                    block_info* node = reinterpret_cast<block_info*>(pools[i] + j * block_layout.size);
                    if (node->ptr.is<flux>())
                    {
                        count++;
                    }
                }
            }
            return count;
        }
        inline bool empty() const {
            return allocated_count() == 0;
        }
        inline bool full() const {
            return top == nullptr;
        }
        inline size_t number_of_pools() const {
            return pools.length();
        }
        size_t size_of_pool(size_t pool_number) const {
            if (pool_number >= 63)
            {
                stn::throw_logic_error("Error: requested pool with size of 1<<{} is too big to exist", pool_number);
            }
            else
            {
                return size_t(1) << pool_number;  // promote 1 to size_t
            }
        }
        //type safe emplace that checks the type_id first
        template<typename U, typename ...Args>
        flux_token<U> emplace(Args&&...args)  requires std::constructible_from<U, Args&&...> {

            if (type_map::type_id::make_type_id<U>() != object_id)
            {
                stn::throw_range_exception("cannot emplace object of type {} in pool of type {} ", typeid(U).name(), stored_type_info->name());
            }
            return emplace_unchecked<U>(std::forward<Args>(args)...);
        }
        //non type safe version of emplace that assumes that U is the allocated type
        template<typename U, typename ...Args>
        flux_token<U> emplace_unchecked(Args&&...args)  requires std::constructible_from<U, Args&&...> {
            if (full()) {
                create_pool();
            }
            //since top is always in free state top->next is in free state
            block_info* node = top;
            U* element = reinterpret_cast<U*>(get_element(node));
            new (element) U(std::forward<Args>(args)...);
            top = &top->ptr.as<block_info>();
            node->ptr.from<flux>(this);
            return flux_token<U>(element);
        }
        void clear() {
            //first check that all nodes are free
            if (!empty())
            {
                //       throw std::logic_error("all elements must be free before the allocator is destroyed");
            }
            for (std::byte* pool : pools)
            {
                delete[] pool;
            }
            pools.clear();
            top = nullptr;

        }


    private:
        // Computes pointer to the `block_info` struct that precedes this object in memory.
        // Invariant: every valid token points to a live object allocated by `flux`.
        inline block_info* get_block_info(void* element) {
            return reinterpret_cast<block_info*>(reinterpret_cast<std::byte*>(element) - block_info_gap);
        }

        // Get pointer to data from block_info pointer
        void* get_element(block_info* metadata) {
            return reinterpret_cast<void*>(reinterpret_cast<std::byte*>(metadata) + block_info_gap);
        }
        //reasigns all ptrs to the flux object
        void reassign_allocator_ptrs() noexcept {
           
            // iterate pools that were moved into this->pools
            // note: pools now contains the arrays previously owned by 'other'
            for (size_t pool_index = 0; pool_index < number_of_pools(); ++pool_index) {
                size_t length = size_of_pool(pool_index);
                std::byte* pool_ptr = pools[pool_index];
                for (size_t i = 0; i < length; ++i) {
                    block_info* node = reinterpret_cast<block_info*>(pool_ptr + i * block_layout.size);
                    //if node points to a flux reasign it
                    if (node->ptr.is<flux>()) {
                        node->ptr.from<flux>(this);
                    }
                }
            }
        }

        void push_to_free_list(block_info* node) {
            //node satifies invariants because top is free
            node->ptr.from<block_info>(top);
            //invariants satified since node satisfies invariants
            top = node;
        }
        // Allocates a new pool with exponentially increasing block count.
        // Each block is initialized as free and linked into the free list.
        void create_pool() {
            size_t new_pool_size = size_of_pool(number_of_pools());
            std::byte* newPool = new std::byte[block_layout.size * new_pool_size];
            pools.push(newPool);
            for (size_t i = 0; i < new_pool_size; i++) {
                block_info* node = reinterpret_cast<block_info*>(newPool + i * block_layout.size);
                push_to_free_list(node);
            }
        }

        
        void destroy_unchecked(void* ptr) {
            block_info* node = get_block_info(ptr);
            destructor(ptr);
            push_to_free_list(node);
        }
        void(*destructor)(void*);
        const std::type_info* stored_type_info;
        stn::memory::layout block_layout;
        type_map::type_id object_id;
        stn::array<std::byte*> pools;

        // block_info inline_stack always nullptr or in free state
        block_info* top;
        template<typename T>
        friend struct flux_token;

    };

   

}
