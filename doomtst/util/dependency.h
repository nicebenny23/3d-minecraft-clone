#pragma once
#include "topo_sort.h"    // dag_sort_indices
#include "type_index.h"   // dense_type_system
#include "typelist.h"     // TypeList::for_each
#include "Option.h"       // stn::Option
#include "type_tag.h"     // has_tag
#include <stdexcept>
#include <type_traits>

namespace Depends {
    struct dependency_first {};
    struct dependency_last {};

    struct NoDependencyTag {};

    // Concept to detect nested type
    template<typename T>
    concept HasDependencies = requires { typename T::Dependencies; };

    template<typename T>
    concept HasDependencyTag = requires { typename T::DependencyTag; };

    // Default (fallback)
    template<typename T>
    struct GetDependencies {
        using type = TypeList::TypeList<>;
    };
    template<HasDependencies T>
    struct GetDependencies<T> {
        using type = typename T::Dependencies;
    };
    // Default (fallback)
    template<typename T>
    struct GetDependencyTag {
        using type = NoDependencyTag;
    };
    template<HasDependencyTag T>
    struct GetDependencyTag<T> {
        using type = typename T::DependencyTag;
    };
    template<typename T>
    constexpr bool is_first_tag = std::same_as<dependency_first, T>;
    template<typename T>
    constexpr bool is_last_tag = std::same_as<dependency_first, T>;
    template<typename T>
    constexpr bool has_first_tag = is_first_tag<typename GetDependencyTag<T>::type>;
    template<typename T>
    constexpr bool has_last_tag = is_last_tag<typename GetDependencyTag<T>::type>;

    
    template<typename Current, typename Accum>
    struct collect_impl;

    // current empty -> done, result is Accum
    template<typename Accum>
    struct collect_impl<TypeList::None, Accum> {
        using type = Accum;
    };

    // General case: Current = TypeList<Head, Tail...>
    template<typename Head, typename... Tail, typename Accum>
    struct collect_impl<TypeList::TypeList<Head, Tail...>, Accum> {
    private:
        // tail as a TypeList
        using TailList = TypeList::TypeList<Tail...>;
        // Head's direct dependencies
        using HeadDeps = typename GetDependencies<Head>::type;
        // If not seen, add to list 
        using newAccum = typename TypeList::append_unique_v<Accum, Head>;
        // If not seen, enqueue HeadDeps after Tail
        static constexpr bool seen = TypeList::contains_v<Accum,Head>;
        //non_seen
        using newCurrent = std::conditional_t<
            seen,
            TailList,
            TypeList::concat_all_t<TailList, HeadDeps> 
        >;
    public:
        using type = typename collect_impl<newCurrent, newAccum>::type;
    };
    // collect_all_dependencies<T> starts the BFS with direct dependencies of T
    template<typename T>
    struct collect_all_dependencies {
    private:
        using direct = typename GetDependencies<T>::type; // may be TypeList<>
    public:
        using type = typename collect_impl<direct, TypeList::None>::type;
    };

    template<typename T>
    using collect_all_dependencies_t = typename collect_all_dependencies<T>::type;      // Recursion check: returns true if B is in the transitive dependencies of A
        template<typename A, typename B>
        constexpr bool has_dependency() {
            return TypeList::contains_v<collect_all_dependencies_t<A>,B>;
        }
       
        template<typename A, typename B>
        constexpr bool is_before() {
            using TagA = typename GetDependencyTag<A>::type;
            using TagB = typename GetDependencyTag<B>::type;

            if constexpr (std::is_same_v<TagA, dependency_first>) return true;
            if constexpr (std::is_same_v<TagB, dependency_first>) return false;
            if constexpr (std::is_same_v<TagA, dependency_last>) return false;
            if constexpr (std::is_same_v<TagB, dependency_last>) return true;

            // delegate recursion to helper
            return has_dependency<A, B>();
        }
        template<typename T>
        concept DependencyOrdered = !has_dependency<T, T>();
       
        template<typename A, typename B>
        constexpr bool is_comparable() {
            return is_before<A, B> || is_before<B, A>;
        }
       


    struct DependencySystem {
        stn::array<uint32_t> sortedActive;

        template<typename T>
        requires DependencyOrdered<T>
        void push() {
           
            auto [tid, is_new] = typeSystem.insert<T>();
            if (!is_new)
            {
                return;
            }
            using Deps = typename GetDependencies<T>::type;
            stn::array<uint32_t> dependees;     // types that T depends on
            
            // Collect dependees from Deps list
            TypeList::for_each<Deps>::template apply<CollectDependees>(dependees, *this);

            // Handle tags
            if constexpr (has_first_tag<T>) {
                if constexpr (!std::is_same_v<Deps, TypeList::TypeList<>>)
                    throw std::runtime_error("dependency_first types cannot have Dependencies");
                builder.push(tid.id, DagBuilder<uint32_t>::PushType::First);
            }
            else if constexpr (has_last_tag<T>) {
                builder.push(tid.id, DagBuilder<uint32_t>::PushType::Last);
            }
            else {
                builder.push(tid.id, dependees, stn::array<uint32_t>());
            }

            // Update sorted list automatically

            sortedActive = dag_sort(builder.getFiltered());
        }
        const type_map::type_indexer<> indexer() const {
            return typeSystem;
        }
        Dag<uint32_t> filtered() {

            return (builder.getFiltered());
        }
    private:
        struct CollectDependees {
            template<typename Dep>
            static void call(stn::array<uint32_t>& dependees, DependencySystem& sys) {
                auto [depId, _] = sys.typeSystem.insert<Dep>();
                dependees.push(depId.id);
            }
        };

        DagBuilder<uint32_t> builder;
        type_map::type_indexer<> typeSystem;

    };

} // namespace Depends
