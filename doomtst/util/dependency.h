#pragma once
#include "type_index.h"    // dense_type_system::insert<T>() → (Id,bool)
#include "dynamicarray.h"  // Cont::array
#include "topo_sort.h"     // Dag, Edge, dag_sort_indices
#include "typelist.h"      // TypeList and for_each
#include "bitset.h"
namespace Depends{
template<typename T>
concept HasDependencies = requires { typename T::Dependencies; };

template<typename T>
using Dependencies = std::conditional_t<
    HasDependencies<T>,
    typename T::Dependencies,
    TypeList::TypeList<>
>;

struct DependencySystem {
    // After each push, this holds only the types you explicitly pushed,
    // in an order that respects Dependencies<T>.
    Cont::array<size_t> sortedActive;

    template<typename T>
    void push() {
        // 1) Register T
        auto [tid, newly] = typeSystem.insert<T>();
        if (!newly) return;

        // 2) Mark T active
        activeList.push(tid.value);

        // 3) Prepare graph node
        ensureNode(tid.value);
        graph.list[tid.value].pred.clear();

        // 4) Wire in each Dep → T
        using Deps = Dependencies<T>;
        TypeList::for_each<Deps>::template apply<DepLinker>(tid.value, *this);

        // 5) Rebuild sortedActive
        rebuildSorted();
    }

private:
    // Functor for TypeList::for_each: adds one Dep → target edge
    struct DepLinker {
        template<typename Dep>
        static void call(size_t target, DependencySystem& sys) {
            auto [did, depNew] = sys.typeSystem.insert<Dep>();
            sys.ensureNode(did.value);
            sys.ensureNode(target);
            sys.graph.list[target].pred.push(did.value);
        }
    };

    // Ensure graph.list has at least idx+1 entries
    void ensureNode(size_t idx) {
        while (graph.list.length <= idx)
            graph.list.push(Edge{});
    }

    // Topo-sort the full graph, then filter to only activeList
    void rebuildSorted() {
        sortedActive.destroy();

        auto allNodes = dag_sort_indices(graph);

        // Build a quick lookup table of “is this node active?”
        bitset::bitset isActive(graph.length(), false);
        for (auto id : activeList) {
            if (id < isActive.bits)
                isActive.set(id);
        }

        // Filter
        for (auto node : allNodes) {
            if (isActive[node])
                sortedActive.push(node);
        }
    }

    // Data
    type_id::dense_type_system typeSystem;
    Dag                         graph;
    Cont::array<size_t>         activeList;
};
}