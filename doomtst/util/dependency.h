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

    // Helper: get nested Dependencies or empty list if not present
    template <typename T, typename = void>
    struct GetDependencies {
        using type = TypeList::TypeList<>;
    };

    template <typename T>
    struct GetDependencies<T, std::void_t<typename T::Dependencies>> {
        using type = typename T::Dependencies;
    };

    // Helper: get nested DependencyTag or void (no tag) if not present
    template <typename T, typename = void>
    struct GetDependencyTag {
        struct type {}; // no tag
    };

    template <typename T>
    struct GetDependencyTag<T, std::void_t<typename T::DependencyTag>> {
        using type = typename T::DependencyTag;
    };

    struct DependencySystem {
        stn::array<uint32_t> sortedActive;

        template<typename T>
        void push() {
           
            auto [tid, isNew] = typeSystem.insert<T>();
            if (builder.pushed.contains(tid.id))
            {
                return;
            }
            using Deps = typename GetDependencies<T>::type;
            using Tag = typename GetDependencyTag<T>::type;

            stn::array<uint32_t> dependees;     // types that T depends on
            stn::array<uint32_t> dependencies;  // unused but needed by dag_builder API

            // Collect dependees from Deps list
            TypeList::for_each<Deps>::template apply<CollectDependees>(dependees, *this);

            // Handle tags
            if constexpr (std::is_same_v<Tag, dependency_first>) {
                if constexpr (!std::is_same_v<Deps, TypeList::TypeList<>>)
                    throw std::runtime_error("dependency_first types cannot have Dependencies");
                builder.push(tid.id, DagBuilder<uint32_t>::PushType::First);
            }
            else if constexpr (std::is_same_v<Tag, dependency_last>) {
                builder.push(tid.id, DagBuilder<uint32_t>::PushType::Last);
            }
            else {
                builder.push(tid.id, dependees, dependencies);
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
