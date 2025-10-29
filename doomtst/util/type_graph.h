#pragma once

#include "topo_sort.h"              // DAG with only successors (succs)
#include "type_index.h"            // dense_type_system
#include "typelist.h"              // TypeList utilities
#include <type_traits>
#include <utility>

// Concept: checks if a type has a nested type named Dependencies
template<typename T>
concept HasDependencies = requires { typename T::Dependencies; };

// Concept: checks if a type has a nested type named Dependees
template<typename T>
concept HasDependees = requires { typename T::Dependees; };

// Get dependencies if they exist, else empty list
template<typename T>
using Dependencies = std::conditional_t<
    HasDependencies<T>,
    typename T::Dependencies,
    TypeList::TypeList<>
>;

// Get dependees if they exist, else empty list
template<typename T>
using Dependees = std::conditional_t<
    HasDependees<T>,
    typename T::Dependees,
    TypeList::TypeList<>
>;

// Builds a type-indexed DAG from pushed types
struct TypeDag {
    Dag<size_t> graph;
    type_id::type_indexer typeSystem;

    template<typename T>
    void push() {
        auto [tid, inserted] = typeSystem.insert<T>();
        if (tid.id >= graph.length()())
            graph.resize(tid.id + 1);

        // Connect Dep -> T
        TypeList::for_each<Dependencies<T>>::template apply<DepLinker>(tid.id, *this);

        // Connect T -> Dependee
        TypeList::for_each<Dependees<T>>::template apply<DependeeLinker>(tid.id, *this);
    }

private:
    // Dep -> T
    struct DepLinker {
        template<typename Dep>
        static void call(size_t target, TypeDag& sys) {
            auto maybe = sys.typeSystem.find<Dep>();
            if (!maybe) return;
            size_t from = maybe.id();
            if (from >= sys.graph.length()())
                sys.graph.resize(from + 1);
            sys.graph.list[from].succs.push(target); 
        }
    };

    // T -> Dependee
    struct DependeeLinker {
        template<typename Dep>
        static void call(size_t source, TypeDag& sys) {
            auto maybe = sys.typeSystem.find<Dep>();
            if (!maybe) return;
            size_t to = maybe.id();
            if (to >= sys.graph.length()())
                sys.graph.resize(to + 1);
            sys.graph.list[source].succs.push(to); // Use `succs`
        }
    };
};
