#pragma once
#include <utility>
#include <iterator>
#include <concepts>
#include <type_traits>
#include <vector>
#include <functional>
#include "pair.h"
#include <stdexcept>
#include "Option.h"

namespace stn {
    template<typename Func, typename Arg>
    concept map_on =
        std::invocable<Func&, Arg> &&
        (!std::same_as<std::invoke_result_t<Func&, Arg>, void>);
    // --- minimal Range concept ---
    template<typename RangeType>
    concept Range = requires(RangeType & r) {
        { std::begin(r) } -> std::forward_iterator;
        { std::end(r) }   -> std::sentinel_for<decltype(std::begin(r))>;
    };
    template<Range R, typename Adaptor>
        requires requires(Adaptor&& a, R&& r) {
            { std::forward<Adaptor>(a)(std::forward<R>(r)) };
    }
    constexpr auto operator|(R&& r, Adaptor&& a)
        -> decltype(std::forward<Adaptor>(a)(std::forward<R>(r))) {
        return std::forward<Adaptor>(a)(std::forward<R>(r));
    }

    // keep/reference for lvalues, makes it owning for other values

    template<Range Source, typename Func>
        requires requires(Func f, std::iter_value_t<std::ranges::iterator_t<Source&>> const& elem) {
            { f(elem) } -> std::same_as<void>; // func must accept const ref and return void
    }struct InspectView;
    template<Range Source, typename Func>
        struct MapView;
    template<Range Source, typename Pred> struct FilterView;
    template<Range Source> struct EnumView;
    template<Range A, Range B> struct ZipView;

    //T->T owning
    //T&->T& refrence
    //exactly what we want
    template<typename T>
    using stored_range_t = std::conditional_t<std::is_lvalue_reference_v<T>, T, std::decay_t<T>>;

    template<Range RangeType>
    struct range {

        using stored_type = stored_range_t<RangeType>;
        stored_type stored_range;

        using iterator = std::ranges::iterator_t<stored_type>;
        using const_iterator = std::ranges::iterator_t<const stored_type>;
        iterator begin() { return std::begin(stored_range); }
        iterator end() { return std::end(stored_range); }
        const_iterator begin() const { return std::begin(stored_range); }
        const_iterator end()   const { return std::end(stored_range); }

        using value_type = std::ranges::range_value_t<stored_type>;
       


        template<typename R>
            requires std::constructible_from<stored_type, R&&>
        explicit range(R&& r) : stored_range(std::forward<R>(r)) {}
        //R&& is either R or L value 
        //Pass the value along by forwarding so it doesent decay

        constexpr bool empty() const { return begin() == end(); }

        constexpr size_t length() const {
            return static_cast<size_t>(std::distance(begin(), end()));
        }

        template<typename T, typename F>
        constexpr auto fold(T init, F&& func) {
            for (auto&& v : stored_range) init = func(init, v);
            return init;
        }

        //you cannot make it just a const version because they can only deduce arguments when it is an extenral fyntction
        template<map_on<std::ranges::range_reference_t<stored_type>> Func>
        range<MapView<stored_type, std::decay_t<Func>>> map(Func&& f)&& {
            using View = MapView<stored_type, std::decay_t<Func>>;
            View v(std::forward<stored_type>(stored_range), std::forward<Func>(f));
            return range<View>(std::move(v));                       
        }

        // lvalue map
        template<map_on<const std::ranges::range_reference_t<stored_type>> Func>
        range<MapView<const stored_type&, std::decay_t<Func>>> map(Func&& f) const& {
            using View = MapView<const stored_type&, std::decay_t<Func>>;
            //passing as && would be dangerous as you would store asT&&
            
            return range<View>(View (stored_range, std::forward<Func>(f)));
        }
        template<typename Func>
        auto inspect(Func&& f)&& {
            using View = InspectView<stored_type, std::decay_t<Func>>;
            return range<View>(View(std::move(stored_range), std::forward<Func>(f)));
        }

        // Lvalue map: preserves the original range
        template<typename Func>
        auto inspect(Func&& f) const& {
            using View = InspectView<const stored_type&, std::decay_t<Func>>;
            return range<View>{
                View{ stored_range, std::forward<Func>(f) }
            };
        }
        template<std::predicate<value_type&> Pred>
        range<FilterView<stored_type, std::decay_t<Pred>>> filter(Pred&& p)&& {
            using View = FilterView<stored_type, std::decay_t<Pred>>;
            // if stored_type is a copy then we can refrence the orignal otherwise if it owns it we should steal it.

            return range<View>(View(stored_range, std::forward<Pred>(p)));
        }

        // lvalue filter
        template<std::predicate<const value_type&> Pred>
        range<FilterView<const stored_type&, std::decay_t<Pred>>>  filter(Pred&& p) const& {
            using View = FilterView<const stored_type&, std::decay_t<Pred>>;
            return range<View>(View(stored_range, std::forward<Pred>(p)));
        }
        auto enumerate()&& {
            using View = EnumView<stored_type>;
            return range<View>{
                View{ std::move(stored_range) }
            };
        }


        auto enumerate() const& {
            using View = EnumView<const stored_type&>;
            return range<View>{
                View{ stored_range}
            };
        }

        template<Range Other>
        auto zip(Other&& o)&& {
            return range{ ZipView<RangeType, Other>{std::move(stored_range),std::move(o) }};
        }
        template<Range Other>
        auto zip(Other&& o) const& {
            return range{ ZipView<const RangeType&, const Other&>{ (stored_range), o } };
        }

        template<template<typename...> class Container>
            requires std::constructible_from<Container<value_type>, iterator, iterator>
        Container<value_type> into() const {
            return Container<value_type>{ begin(), end()};
        }
  
        template<typename Func>
        void for_each(Func&& f) {
            for (auto&& element : stored_range) {
                f(element);
            };
        }


        template<typename Pred>
        bool any(Pred&& pred) const {
            for (auto&& element : stored_range)
            {
                if (pred(element))
                {
                    return true;
                }
            }
            return false;
        }

        template<typename Pred>
        bool all(Pred&& pred) const {
            for (auto&& element : stored_range)
            {
                if (!pred(element))
                {
                    return false;
                }
            }
            return true;
        }
        template<typename T>
        bool contains(const T& val) const {
            for (auto&& e : stored_range) if (e == val) return true;
            return false;
        }
        template<typename Pred>
        size_t count(Pred&& pred) const {
            size_t cnt = 0;
            for (auto&& element : stored_range) {
                if (pred(element)) {
                    cnt++;
                };
            }
            return cnt;
        }

        void Debug() const {
            for (auto&& e : stored_range) {
            }
        }

        template<typename Pred>
        Option<value_type> find(Pred&& pred) const {
            for (auto&& element : stored_range)
                if (pred(element))
                {
                    return element;
                }
            return None;
        }
        bool operator==(const range& other) const {
            auto it1 = begin();
            auto it2 = other.begin();
            auto end1 = end();
            auto end2 = other.end();

            for (; it1 != end1 && it2 != end2; ++it1, ++it2) {
                if (*it1 != *it2)
                    return false;
            }

            return it1 == end1 && it2 == end2;
        }
        bool operator!=(const range& other) const {
            return !(*this == other);
        }
    };
    //R&&->R
    //R&->R&
    //thus R&& is owning,and R& is a refrence exactly what we want
    template<Range R>
    range(R&&) -> range<R>;

    // ---------------- MapView ----------------
    template<Range Source, typename Func>
    struct MapView {
        using stored_t = stored_range_t<Source>;
        stored_t source;
        Func func;
        //same functoinality as above,takes ownership of R values and refrences L values
        template<typename S>
        MapView(S&& s, Func f) requires std::constructible_from<stored_t,S&&>
            : source(std::forward<S>(s)), func(std::move(f)) {
        }
        // Iterator template: Iter = underlying iterator type, FuncPtr = Func* or const Func*
        template<typename iter_t, typename FuncPtr>
        struct Iterator {
            iter_t iter;
            FuncPtr func_ptr;
            using base_category = typename std::iterator_traits<iter_t>::iterator_category;

            // Determine value_type and reference type
            using value_type = std::invoke_result_t<Func&, std::iter_reference_t<iter_t>>;
            using reference = value_type;
            using pointer = void;
            using difference_type = std::iter_difference_t<iter_t>;

            using iterator_category = std::conditional_t<
                std::derived_from<base_category, std::contiguous_iterator_tag>,
                std::random_access_iterator_tag,
                base_category
            >;
            // compute result of invoking func on the iterator reference
         
            static_assert(!std::is_void_v<value_type>, "MapView Func must not return void (use InspectView for void).");
            static constexpr bool is_bidirectional = std::derived_from<iterator_category, std::bidirectional_iterator_tag>;
            static constexpr bool is_random_access = std::derived_from<iterator_category, std::random_access_iterator_tag>;


            reference operator*() const {
                return std::invoke(*func_ptr, *iter);
            }
            Iterator& operator++() { ++iter; return *this; }
            Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

            template<typename OtherIter, typename OtherFuncPtr>
            bool operator==(Iterator<OtherIter, OtherFuncPtr> const& o) const { return iter == o.iter; }
            template<typename OtherIter, typename OtherFuncPtr>
            bool operator!=(Iterator<OtherIter, OtherFuncPtr> const& o) const { return iter != o.iter; }
            //bidirectional

            Iterator& operator--() requires is_bidirectional { --iter; return *this; }
            Iterator operator--(int) requires is_bidirectional { Iterator tmp = *this; --(*this); return tmp; }

            //random access
            reference operator[](difference_type n) const requires is_random_access { return std::invoke(*func_ptr, iter[n]); }
            Iterator& operator+=(difference_type n) requires is_random_access { iter += n; return *this; }
            Iterator& operator-=(difference_type n) requires is_random_access { iter -= n; return *this; }
            Iterator operator+(difference_type n) const requires is_random_access { return Iterator{ iter + n, func_ptr }; }
            Iterator operator-(difference_type n) const requires is_random_access { return Iterator{ iter - n, func_ptr }; }
            difference_type operator-(const Iterator& other) const requires is_random_access { return iter - other.iter; }

            Iterator() = default;
            Iterator(iter_t iter_start, FuncPtr function_ptr) : iter(iter_start), func_ptr(function_ptr) {}

        };

        using iter_t = std::ranges::iterator_t<stored_t&>;
        using const_iter_t = std::ranges::iterator_t<const stored_t&>;
        //you cannot take &* so we need to remove the refrence
        using pointed_t = std::remove_reference_t<Func>;
        using iterator = Iterator<iter_t, pointed_t*>;
        using const_iterator = Iterator<const_iter_t, const pointed_t*>;

        iterator begin() { return iterator{ std::begin(source), &func }; }
        iterator end() { return iterator{ std::end(source), &func }; }
        const_iterator begin() const { return const_iterator{ std::begin(source), &func }; }
        const_iterator end()   const { return const_iterator{ std::end(source), &func }; }
    };

    template<Range Source, typename Func>
        requires requires(Func f, std::iter_value_t<std::ranges::iterator_t<Source&>> const& elem) {
            { f(elem) } -> std::same_as<void>; // func must accept const ref and return void
    }
    struct InspectView {
        using stored_t = stored_range_t<Source>;
        stored_t source;
        Func func;
        //same functoinality as above,takes ownership of R values and refrences L values
        template<typename S>
        InspectView(S&& s, Func f)
            : source(std::forward<S>(s)), func(std::move(f)) {
        }
        // Iterator template: Iter = underlying iterator type, FuncPtr = Func* or const Func*
        template<typename iter_t, typename FuncPtr>
        struct Iterator {
            iter_t iter;
            FuncPtr func_ptr;
            using base_category = typename std::iterator_traits<iter_t>::iterator_category;

            // Determine value_type and reference type
            using value_type = std::iter_value_t<iter_t>;
            using reference = value_type&;
            using pointer = void;
            using difference_type = std::iter_difference_t<iter_t>;

            using iterator_category = std::conditional_t<
                std::derived_from<base_category, std::contiguous_iterator_tag>,
                std::random_access_iterator_tag,
                base_category
            >;

            static constexpr bool is_bidirectional = std::derived_from<iterator_category, std::bidirectional_iterator_tag>;
            static constexpr bool is_random_access = std::derived_from<iterator_category, std::random_access_iterator_tag>;


            decltype(auto)  operator*() const {
                std::invoke(*func_ptr, *iter);
                return *iter;
            }
            Iterator& operator++() { ++iter; return *this; }
            Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

            template<typename OtherIter, typename OtherFuncPtr>
            bool operator==(Iterator<OtherIter, OtherFuncPtr> const& o) const { return iter == o.iter; }
            template<typename OtherIter, typename OtherFuncPtr>
            bool operator!=(Iterator<OtherIter, OtherFuncPtr> const& o) const { return iter != o.iter; }
            //bidirectional

            Iterator& operator--() requires is_bidirectional { --iter; return *this; }
            Iterator operator--(int) requires is_bidirectional { Iterator tmp = *this; --(*this); return tmp; }

            //random access
            reference operator[](difference_type n) const requires is_random_access { std::invoke(*func_ptr, iter[n]); return iter[n]; }
            Iterator& operator+=(difference_type n) requires is_random_access { iter += n; return *this; }
            Iterator& operator-=(difference_type n) requires is_random_access { iter -= n; return *this; }
            Iterator operator+(difference_type n) const requires is_random_access { return Iterator{ iter + n, func_ptr }; }
            Iterator operator-(difference_type n) const requires is_random_access { return Iterator{ iter - n, func_ptr }; }
            difference_type operator-(const Iterator& other) const requires is_random_access { return iter - other.iter; }

            Iterator() = default;
            Iterator(iter_t iter_start, FuncPtr function_ptr) : iter(iter_start), func_ptr(function_ptr) {}

        };

        using iter_t = std::ranges::iterator_t<stored_t&>;
        using const_iter_t = std::ranges::iterator_t<const stored_t&>;
        //you cannot take &* so we need to remove the refrence
        using pointed_t = std::remove_reference_t<Func>;
        using iterator = Iterator<iter_t, pointed_t*>;
        using const_iterator = Iterator<const_iter_t, const pointed_t*>;

        iterator begin() { return iterator{ std::begin(source), &func }; }
        iterator end() { return iterator{ std::end(source), &func }; }
        const_iterator begin() const { return const_iterator{ std::begin(source), &func }; }
        const_iterator end()   const { return const_iterator{ std::end(source), &func }; }
    };
    // ---------------- FilterView ----------------
    //predicate checked in the .method
    template<Range Source, typename Pred>
    struct FilterView {
        using stored_t = stored_range_t<Source>;
        stored_t source;
        Pred pred;

        template<typename S>
        FilterView(S&& s, Pred p) requires std::constructible_from<stored_t, S&&>
            : source(std::forward<S>(s)), pred(std::move(p)) {
        }
        //while it is techniccly  bidiectional making it bidirectioal would slow it down
        template<typename iter_t, typename PredType>
        struct Iterator {
            iter_t curr;
            iter_t end;
            PredType pred_ptr;

            using value_type = std::iter_value_t<iter_t>;
            using reference = std::iter_reference_t<iter_t>;
            using pointer = value_type*;
            using difference_type = std::iter_difference_t<iter_t>;
            using iterator_category = std::forward_iterator_tag;
            Iterator() = default;
            Iterator(iter_t current, iter_t init_end, PredType pred) : curr(current), end(init_end), pred_ptr(pred) {
                while (curr != end && !std::invoke(*pred_ptr, *curr)) ++curr;
            }

            decltype(auto) operator*() const { return *curr; }

            Iterator& operator++() {
                do { ++curr; } while (curr != end && !std::invoke(*pred_ptr, *curr));
                return *this;
            }
            Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

            template<typename OIter, typename OPredType>
            bool operator==(Iterator<OIter, OPredType> const& other) const { return curr == other.curr; }
            template<typename OIter, typename OPredType>
            bool operator!=(Iterator<OIter, OPredType> const& other) const { return curr != other.curr; }
        };

        using iter_t = std::ranges::iterator_t<stored_t&>;
        using const_iter_t = std::ranges::iterator_t<const stored_t&>;

        //you cannot take &* so we need to remove the refrence
        using pointed_t = std::remove_reference_t<Pred>;
        using iterator = Iterator<iter_t, pointed_t*>;
        using const_iterator = Iterator<const_iter_t, const pointed_t*>;

        iterator begin() { return iterator{ std::begin(source), std::end(source), &pred }; }
        iterator end() { return iterator{ std::end(source), std::end(source), &pred }; }
        const_iterator begin() const { return const_iterator{ std::begin(source), std::end(source), &pred }; }
        const_iterator end()   const { return const_iterator{ std::end(source), std::end(source), &pred }; }
    };

    // ---------------- EnumView ----------------

    template<Range Source>
    struct EnumView {
        using stored_t = stored_range_t<Source>;
        stored_t source;

        template<typename S>
        EnumView(S&& s) : source(std::forward<S>(s)) {}
        using iter_t = std::ranges::iterator_t<stored_t&>;
        using const_iter_t = std::ranges::iterator_t<const stored_t&>;
        template<typename iter_t>
        struct Iterator {
            iter_t iter;
            size_t index = 0;
            using value_type = stn::indexed<std::iter_value_t<iter_t>>;
            using pointer = void;
            using reference = stn::indexed<std::iter_reference_t<iter_t>>;
            using difference_type = std::iter_difference_t<iter_t>;


            using base_category = typename std::iterator_traits<iter_t>::iterator_category;
            using iterator_category = std::conditional_t<
                std::derived_from<base_category, std::contiguous_iterator_tag>,
                std::random_access_iterator_tag,
                base_category
            >;
            static constexpr bool is_bidirectional = std::derived_from<iterator_category, std::bidirectional_iterator_tag>;
            static constexpr bool is_random_access = std::derived_from<iterator_category, std::random_access_iterator_tag>;

            auto operator*() const {
                return reference{ index, *iter };

            }
            Iterator() = default;
            Iterator(iter_t i) : iter(i), index(0) {}

            Iterator& operator++() { ++iter; ++index; return *this; }
            Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

            template<typename OtherIter>
            bool operator==(Iterator<OtherIter> const& o) const { return iter == o.iter; }
            template<typename OtherIter>
            bool operator!=(Iterator<OtherIter> const& o) const { return iter != o.iter; }

            Iterator& operator--() requires is_bidirectional { --iter; --index; return *this; }
            Iterator operator--(int) requires is_bidirectional { Iterator tmp = *this; --(*this); return tmp; }

            reference operator[](difference_type n) const requires is_random_access { return reference{ index, *iter }; }
            Iterator& operator+=(difference_type n) requires is_random_access { iter += n; index += n; return *this; }
            Iterator& operator-=(difference_type n) requires is_random_access { iter -= n; index -= n; return *this; }
            Iterator operator+(difference_type n) const requires is_random_access { return Iterator{ iter + n, index + n }; }
            Iterator operator-(difference_type n) const requires is_random_access { return Iterator{ iter - n, index - n }; }
            difference_type operator-(const Iterator& other) const requires is_random_access { return Iterator{ iter - other.iter, index - other.index }; }

        };
        using iterator = Iterator<iter_t>;
        using const_iterator = Iterator<const_iter_t>;

        iterator begin() { return iterator{ std::begin(source) }; }
        iterator end() { return iterator{ std::end(source) }; }
        const_iterator begin() const { return const_iterator{ std::begin(source) }; }
        const_iterator end()   const { return const_iterator{ std::end(source) }; }
    };

    // ---------------- ZipView (stops at shortest) ----------------
    template<Range A, Range B>
    struct ZipView {
        using stored_a = stored_range_t<A>;
        using stored_b = stored_range_t<B>;
        stored_a a;
        stored_b b;
        ZipView(A&& a_iter, B&& b_iter) : a(std::forward<A>(a_iter)), b(std::forward<B>(b_iter)) {}

        template<typename IterA, typename IterB>
        struct Iterator {



            IterA it1;
            IterB it2;
            using value_type = stn::pair<
                std::iter_value_t<IterA>,
                std::iter_value_t<IterB>>;

            using reference = stn::pair<std::iter_reference_t<IterA>, std::iter_reference_t<IterB>>;

            using pointer = void;
            using difference_type = std::iter_difference_t<IterA>;

            Iterator() = default;
            Iterator(IterA i1, IterB i2) : it1(i1), it2(i2) {}

            auto operator*() const {
                return reference{ *it1, *it2 };
            }

            Iterator& operator++() { ++it1; ++it2; return *this; }
            Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

            template<typename OtherA, typename OtherB>
            bool operator==(Iterator<OtherA, OtherB> const& o) const { return it1 == o.it1 || it2 == o.it2; }

            template<typename OtherA, typename OtherB>
            bool operator!=(Iterator<OtherA, OtherB> const& other) const { return !(*this == other); }
        };

        using iter_a = std::ranges::iterator_t<stored_a&>;
        using const_iter_a = std::ranges::iterator_t<const stored_a&>;
        using iter_b = std::ranges::iterator_t<stored_b&>;
        using const_iter_b = std::ranges::iterator_t<const stored_b&>;
        using iterator = Iterator<iter_a, iter_b>;
        using const_iterator = Iterator<const_iter_a, const_iter_b>;

        iterator begin() { return iterator{ std::begin(a), std::begin(b) }; }
        iterator end() { return iterator{ std::end(a), std::end(b) }; }
        const_iterator begin() const { return const_iterator{ std::begin(a), std::begin(b) }; }
        const_iterator end()   const { return const_iterator{ std::end(a), std::end(b) }; }
    };
    template<typename R>
    concept ClearableRange =
        Range<R> &&
        requires(R & r) {
            { r.clear() } -> std::same_as<void>;
    };

    template<ClearableRange C>
    struct DrainRange {
        C&& src;

        using iterator = decltype(std::begin(src));
        using value_type = std::ranges::range_value_t<C>;

        explicit DrainRange(C&& c) noexcept : src(std::move(c)) {}

        iterator begin() noexcept { return std::begin(src); }
        iterator end() noexcept { return std::end(src); }
		bool empty(){
			return begin() == end();
		}
        // Default move/copy
        DrainRange(DrainRange&&) noexcept = default;
        DrainRange& operator=(DrainRange&&) noexcept = default;

        ~DrainRange() {
            src.clear();
        }
    };

    struct DrainAdaptor {
        template<ClearableRange C>
        auto operator()(C&& c) const noexcept
            requires std::is_rvalue_reference_v<C&&>
        {
            return DrainRange<C&&>(static_cast<C&&>(c));
        }
    };

    inline constexpr DrainAdaptor drain{};
    template<typename R>
    concept RandomRange =
        std::ranges::range<R> &&
        std::random_access_iterator<std::ranges::iterator_t<R>> &&
        std::sized_sentinel_for<std::ranges::sentinel_t<R>, std::ranges::iterator_t<R>>;
    template<RandomRange R, typename Compare = std::less<>>
    struct SortAction {
        R& src; 
        Compare comp;

        explicit SortAction(R& r, Compare c = {}) : src(r), comp(c) {
            std::sort(std::begin(src), std::end(src), comp); 
        }

        auto begin() { return std::begin(src); }
        auto end() { return std::end(src); }
    };

    // Adaptor
    struct SortAdaptor {
        constexpr auto operator()() const {
            return [](auto&& r) {
                return SortAction<decltype(r)&&, std::less<>>{ std::forward<decltype(r)>(r) };
                };
        }

        // Custom comparator
        template<typename Compare>
        constexpr auto operator()(Compare comp) const {
            return [comp](auto&& r) {
                return SortAction<decltype(r)&&, Compare>{ std::forward<decltype(r)>(r), comp };
                };
        }
    };

    inline constexpr SortAdaptor sort{};
}