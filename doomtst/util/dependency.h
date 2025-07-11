// DependencySystem.h
#pragma once

#include "type_index.h"    // dense_type_system: insert<T>() → (Id,bool)
#include "dynamicarray.h"  // Cont::array
#include "topo_sort.h"     // Dag, dag_sort_indices
#include "typelist.h"      // TypeList::for_each
#include "bitset.h"        // bitset
#include "Option.h"        // Opt::Option
#include <type_traits>
#include <stdexcept>

namespace Depends {

    //================================================================================
    // USAGE:
    // 1) Define your types/systems with:
    //      using Dependencies = TypeList::TypeList<Dep1, Dep2, ...>;
    //      // optional tag:
    //      using DependencyTag = Depends::dependency_first; // or dependency_last
    // 2) Create a DependencySystem instance and call push<T>() for each type.
    //    You may push in any order; sortedActive will always respect dependencies,
    //    placing types tagged "dependency_first" at the front, and
    //    types tagged "dependency_last" at the end.
    // 3) Iterate over system.sortedActive to run in the correct order.
    //================================================================================

    // Tag a type so it always runs first
    struct dependency_first {};
    // Tag a type so it always runs last
    struct dependency_last {};

    // Extract T::Dependencies or default to empty list
    template<typename T, typename = void>
    struct DependenciesHelper { using type = TypeList::TypeList<>; };
    template<typename T>
    struct DependenciesHelper<T, std::void_t<typename T::Dependencies>> {
        using type = typename T::Dependencies;
    };
    template<typename T>
    using Dependencies = typename DependenciesHelper<T>::type;

    // Extract optional T::DependencyTag or default to void
    template<typename T, typename = void>
    struct TagHelper { using type = void; };
    template<typename T>
    struct TagHelper<T, std::void_t<typename T::DependencyTag>> {
        using type = typename T::DependencyTag;
    };
    template<typename T>
    using DependencyTag = typename TagHelper<T>::type;

    // Core dependency system
    struct DependencySystem {
        // After pushing, holds type IDs in execution order
        Cont::array<size_t> sortedActive;

        template<typename T>
        void push() {
           
            auto [tid, isNew] = typeSystem.insert<T>();
            if (!isNew) return; // already registered

            activeList.push(tid.value);
            graph.list.reach(tid.value).pred.destroy(); // clear old deps

            using Deps = Dependencies<T>;
            using Tag = DependencyTag<T>;

            // Disallow listing other deps on a "first"-tagged type
            if constexpr (std::is_same_v<Tag, dependency_first>) {
                if constexpr (!std::is_same_v<Deps, TypeList::TypeList<>>)
                    throw std::runtime_error("Types tagged dependency_first cannot declare Dependencies");
            }

            // Wire each Dep -> T
            TypeList::for_each<Deps>
                ::template apply<DepLinker>(tid.value, *this);

            // Record tag for final positioning
            if constexpr (std::is_same_v<Tag, dependency_first>)
                firstId = tid.value;
            else if constexpr (std::is_same_v<Tag, dependency_last>)
                lastId = tid.value;

            rebuildSorted();
        }

    private:
        // Helper: for each Dep in T::Dependencies:
        //  - forbid depending on "last"-tagged types
        //  - add graph edge Dep -> target
        struct DepLinker {
            template<typename Dep>
            static void call(size_t target, DependencySystem& sys) {
                using DepTag = DependencyTag<Dep>;
                if constexpr (std::is_same_v<DepTag, dependency_last>)
                    throw std::runtime_error("Cannot depend on a type tagged dependency_last");

                auto [depId, _] = sys.typeSystem.insert<Dep>();
                sys.graph.list[depId.value].succ.push(target);
                sys.graph.list[target].pred.push(depId.value);
            }
        };

        // Recompute sortedActive:
        //  1) topo-sort full graph
        //  2) filter only active IDs
        //  3) move firstId to front, lastId to back
        void rebuildSorted() {
            sortedActive.destroy();
            auto fullOrder = dag_sort_indices(graph);

            bitset::bitset mask(graph.length(), false);
            for (auto id : activeList)
                if (id < mask.bits) mask.set(id);

            for (auto id : fullOrder)
                if (mask[id]) sortedActive.push(id);

            if (firstId) moveToFront(firstId.unwrap());
            if (lastId)  moveToBack(lastId.unwrap());
        }

        // Remove and insert at front/back
        void moveToFront(size_t id) {
            for (size_t i = 0; i < sortedActive.length; ++i) {
                if (sortedActive[i] == id) {
                    sortedActive.deleteind(i);
                    sortedActive.insertind(0, id);
                    return;
                }
            }
        }
        void moveToBack(size_t id) {
            for (size_t i = 0; i < sortedActive.length; ++i) {
                if (sortedActive[i] == id) {
                    sortedActive.deleteind(i);
                    sortedActive.push(id);
                    return;
                }
            }
        }

        // Internals
        Dag                         graph;
        type_id::dense_type_system  typeSystem;
        Cont::array<size_t>         activeList;
        Opt::Option<size_t>         firstId, lastId;
    };

} // namespace Depends
