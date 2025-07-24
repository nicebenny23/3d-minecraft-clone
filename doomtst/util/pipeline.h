#pragma once
#include <utility>   // for std::move, std::forward
#include <iterator>
#include <concepts>
#include "dynamicarray.h"
#include <type_traits>
#include "pair.h"
#include <format>
#include <ranges>
#include <stdexcept>

namespace range {

    template<typename RangeType>
    concept Range = requires(RangeType & r) {
        { std::begin(r) } -> std::forward_iterator;
        { std::end(r) }   -> std::sentinel_for<decltype(std::begin(r))>;
    };

    template<Range Source>
    struct StoreRange {
        using type = std::conditional_t<
            std::is_lvalue_reference_v<Source>,
            std::remove_reference_t<Source>&,
            std::remove_reference_t<Source>
        >;
    };

    template<Range Source, typename Func> struct MapView;
    template<Range Source, typename Pred> struct FilterView;

    template<Range RangeType>
    struct Pipe {
        typename StoreRange<RangeType>::type range;
        using iterator = decltype(std::begin(std::declval<RangeType&>()));
        using const_iterator = decltype(std::begin(std::declval<const RangeType&>()));

        constexpr iterator begin() { return std::begin(range); }
        constexpr iterator end() { return std::end(range); }
        constexpr const_iterator begin() const { return std::begin(range); }
        constexpr const_iterator end() const { return std::end(range); }

        constexpr bool empty() { return begin() == end(); }
        constexpr size_t length() { return std::distance(begin(), end()); }

        template<typename T, typename F>
        auto fold(T init, F func) {
            for (auto&& val : *this) {
                init = func(init, val);
            }
            return init;
        }

        template<typename Pred>
        decltype(auto) filter(Pred p) {
            static_assert(Range<RangeType>, "RangeType must satisfy Range");
            auto new_view = FilterView<RangeType, Pred>(range, std::move(p));
            return Pipe<decltype(new_view)>(std::move(new_view));
        }

        template<typename Func>
        decltype(auto) map(Func f) {
            static_assert(Range<RangeType>, "RangeType must satisfy Range");
            auto new_view = MapView<RangeType, Func>(range, std::move(f));
            return Pipe<decltype(new_view)>(std::move(new_view));
        }

        decltype(auto) enumerate() {
            static_assert(Range<RangeType>, "RangeType must satisfy Range");
            auto new_view = EnumView<RangeType>(range);
            return Pipe<decltype(new_view)>(std::move(new_view));
        }

        template<Range OtherRange>
        decltype(auto) zip(OtherRange&& Oth) {
            static_assert(Range<RangeType>, "RangeType must satisfy Range");
            auto new_view = ZipView<RangeType, OtherRange>(range, std::forward<OtherRange>(Oth));
            return Pipe<decltype(new_view)>(std::move(new_view));
        }

        template<typename Container = stn::array<std::decay_t<decltype(*std::begin(range))>>>
            requires std::constructible_from<Container, decltype(std::begin(range)), decltype(std::end(range))>
        Container into() {
            return Container(begin(), end());
        }

        template<typename Func>
        void for_each(Func&& f) {
            for (auto&& elem : *this) {
                f(elem);
            }
        }

        template<typename RangeType2>
            requires std::constructible_from<RangeType, RangeType2&&>
        Pipe(RangeType2&& r) : range(std::forward<RangeType2>(r)) {}
    };

    template<Range T>
    auto pipe(T&& t) {
        return Pipe<T>{ std::forward<T>(t) };
    }
    template<Range T, typename Pred>
    auto filter(T&& t, Pred pred) {
        return Pipe<T>{ std::forward<T>(t)}.filter(pred);
    }
    template<Range T, typename Map>
    auto map(T&& t, Map mapping) {
        return Pipe<T>{ std::forward<T>(t)}.map(mapping);
    }
    // --- MapView ---

    template<Range Source, typename Func>
    struct MapView {
        typename StoreRange<Source>::type source;  // stores underlying range (ref or value)
        Func func;      // the mapping function

        struct Iterator {
            decltype(std::begin(std::declval<Source&>())) it;
            using value_type = std::decay_t<decltype(*it)>;
            using reference = decltype(*it);
            using pointer = value_type*;
            using difference_type = std::ptrdiff_t;
            using iterator_concept = std::forward_iterator_tag;
            using iterator_category = std::forward_iterator_tag;
            Func* func;

            Iterator() = default; // required for default-constructibility
            Iterator(const Iterator&) = default; // required for copy-constructibility
            inline constexpr Iterator(decltype(it) iter, Func* fn) : it(iter), func(fn) {}
            inline constexpr Iterator& operator++() { ++it; return *this; }
            inline constexpr Iterator operator++(int) { Iterator temp = *this; ++(*this); return temp; }
            inline constexpr decltype(auto) operator*() const { return (*func)(*it); }
            inline bool operator==(const Iterator& oth) const { return it == oth.it; }
            inline constexpr bool operator!=(const Iterator& oth) const { return it != oth.it; }
        };

        using iterator = Iterator;
        using const_iterator = Iterator;

        constexpr iterator begin() { return Iterator(std::begin(source), &func); }
        constexpr iterator end() { return Iterator(std::end(source), &func); }
        constexpr const_iterator begin() const { return Iterator(std::begin(source), &func); }
        constexpr const_iterator end() const { return Iterator(std::end(source), &func); }

        inline MapView(Source s, Func fnc)
            : source(s), func(std::move(fnc)) {
        }
    };

    // --- EnumView ---

    template<Range Source>
    struct EnumView {
        typename StoreRange<Source>::type source;  // stores underlying range (ref or value)

        struct Iterator {
            size_t index;
            decltype(std::begin(std::declval<Source&>())) it;
            using value_type = util::pair<size_t, std::decay_t<decltype(*it)>>;
            using reference = util::pair<size_t, decltype(*it)>;
            using pointer = void;
            using difference_type = std::ptrdiff_t;
            using iterator_concept = std::forward_iterator_tag;
            using iterator_category = std::forward_iterator_tag;

            Iterator() = default; // required for default-constructibility
            Iterator(const Iterator&) = default; // required for copy-constructibility
            reference operator*() const { return { index, *it }; }
            Iterator(decltype(it) iter, size_t idx) : index(idx), it(iter) {}
            inline constexpr Iterator(decltype(it) iter) : it(iter), index(0) {}
            inline constexpr Iterator& operator++() { ++it; index++; return *this; }
            inline constexpr Iterator operator++(int) { Iterator temp = *this; ++(*this); return temp; }
            inline bool operator==(const Iterator& oth) const { return it == oth.it; }
            inline constexpr bool operator!=(const Iterator& oth) const { return it != oth.it; }
        };

        using iterator = Iterator;
        using const_iterator = Iterator;

        constexpr iterator begin() { return Iterator(std::begin(source), 0); }
        constexpr iterator end() { return Iterator(std::end(source), std::distance(std::begin(source), std::end(source))); }
        constexpr const_iterator begin() const { return Iterator(std::begin(source), 0); }
        constexpr const_iterator end() const { return Iterator(std::distance(std::begin(source), std::end(source))); }

        inline EnumView(Source s)
            : source(s) {
        }
    };

    // --- FilterView ---

    template<Range Source, typename Pred>
    struct FilterView {
        typename StoreRange<Source>::type source;  // stores underlying range (ref or value)
        Pred pred;      // the mapping function

        struct Iterator {
            decltype(std::end(std::declval<Source&>())) end;
            decltype(std::begin(std::declval<Source&>())) curr;
            using value_type = std::decay_t<decltype(*curr)>;
            using reference = decltype(*curr);
            using pointer = value_type*;
            using difference_type = std::ptrdiff_t;
            using iterator_concept = std::forward_iterator_tag;
            using iterator_category = std::forward_iterator_tag;
            Pred* pred;

            Iterator() = default; // required for default-constructibility
            Iterator(const Iterator&) = default; // required for copy-constructibility
            inline constexpr Iterator(decltype(curr) curr_iter, decltype(end) end_iter, Pred* fn)
                : curr(curr_iter), end(end_iter), pred(fn) {
                while (curr != end && !(*pred)(*curr)) {
                    curr++;
                }
            }
            inline constexpr Iterator& operator++() {
                do { curr++; } while (curr != end && !(*pred)(*curr));
                return *this;
            }
            inline constexpr Iterator operator++(int) { Iterator temp = *this; ++(*this); return temp; }
            inline constexpr auto operator*() const { return (*curr); }
            bool operator==(const Iterator& oth) const { return curr == oth.curr; }
            bool operator!=(const Iterator& oth) const { return curr != oth.curr; }
        };

        using iterator = Iterator;
        using const_iterator = Iterator;

        inline constexpr iterator begin() { return Iterator(std::begin(source), std::end(source), &pred); }
        inline constexpr iterator end() { return Iterator(std::end(source), std::end(source), &pred); }
        inline constexpr const_iterator begin() const { return Iterator(std::begin(source), std::end(source), &pred); }
        inline constexpr const_iterator end() const { return Iterator(std::end(source), std::end(source), &pred); }

        FilterView(Source s, Pred p)
            : source(s), pred(std::move(p)) {
        }
    };

    // --- ZipView ---

    template<Range FirstRange, Range SecondRange>
    struct ZipView {
        typename StoreRange<FirstRange>::type z1;  // stores underlying range (ref or value)
        typename StoreRange<SecondRange>::type z2;  // stores underlying range (ref or value)

        struct Iterator {
            decltype(std::begin(std::declval<FirstRange&>())) it1;
            decltype(std::begin(std::declval<SecondRange&>())) it2;
            using value_type = util::pair<std::decay_t<decltype(*it1)>, std::decay_t<decltype(*it2)>>;
            using reference = util::pair<decltype(*it1), decltype(*it2)>;
            using pointer = void;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;
            using iterator_concept = std::forward_iterator_tag;

            Iterator() = default; // required for default-constructibility
            Iterator(const Iterator&) = default; // required for copy-constructibility
            inline Iterator(decltype(it1) z1_iter, decltype(it2) z2_iter) : it1(z1_iter), it2(z2_iter) {}
            inline Iterator& operator++() { ++it1; ++it2; return *this; }
            inline constexpr Iterator operator++(int) { Iterator temp = *this; ++(*this); return temp; }
            reference operator*() const noexcept { return reference{ *it1, *it2 }; }
            inline bool operator==(const Iterator& oth) const {
                bool first_equal = (it1 == oth.it1);
                bool second_equal = (it2 == oth.it2);
                if (first_equal != second_equal) {
                    if (!first_equal) {
                        auto dist = std::distance(it1, oth.it1);
                        throw std::logic_error(std::format("Mismatch on second iterator: remaining distance = {}", dist));
                    }
                    else {
                        auto dist = std::distance(it2, oth.it2);
                        throw std::logic_error(std::format("Mismatch on second iterator: remaining distance = {}", dist));
                    }
                }
                return first_equal && second_equal;
            }
            inline bool operator!=(const Iterator& oth) const { return !(*this == oth); }
        };

        using iterator = Iterator;
        using const_iterator = Iterator;

        constexpr inline iterator begin() { return Iterator(std::begin(z1), std::begin(z2)); }
        constexpr inline iterator end() { return Iterator(std::end(z1), std::end(z2)); }
        constexpr inline const_iterator begin() const { return Iterator(std::begin(z1), std::begin(z2)); }
        constexpr inline const_iterator end() const { return Iterator(std::end(z1), std::end(z2)); }

        ZipView(FirstRange r1, SecondRange r2) : z1(std::forward<FirstRange>(r1)), z2(std::forward<SecondRange>(r2)) {}
    };

}