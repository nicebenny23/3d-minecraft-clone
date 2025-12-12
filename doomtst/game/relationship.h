#pragma once
#include "ecs.h"
#include "../util/Option.h"
#include "../util/SparseSet.h"
#include "../util/pipeline.h"
#include <tuple>
#include "../util/counter.h"
namespace Hierarchy {

    struct Parent;
    struct Child : ecs::component{
        bool is_child_of(const ecs::obj& other) const {
            return parent_entity == other;
        }

        ecs::obj parent() const {
            return parent_entity;
        }

    private:
        friend struct Parent;

        explicit Child(ecs::obj& parent)
            : parent_entity(parent) {
        }

        void destroy_hook() {
            parent_entity.get_component<Parent>().remove_child(owner);
        }

        ecs::obj parent_entity;
    };


    struct Parent : ecs::component{
        const array<ecs::obj>& children() const {
            return children_list;
        }

        bool has_child(const ecs::obj& obj) const {
            return children_list.contains(obj);
        }
        size_t child_count() const {
            return children_list.length();
        }

        explicit Parent(ecs::obj& child) {
            children_list.push(child);
        }
        void start() {
            add_child(children_list[0]);
        }

        void add_child(ecs::obj& child) {
            if (child == owner) {
                throw std::logic_error("An object cannot be its own parent.");
            }
            if (children_list.contains(child)) {
                auto& existing = child.get_component<Child>();
                if (existing.parent_entity == owner) {
                    return;
                }
                existing.parent_entity.get_component<Parent>().remove_child(child);
                existing.parent_entity = owner;
            }
            else {
                child.add_component<Child>(owner);
            }
            children_list.push(child);
        }
    private:

        friend struct Child;
        void remove_child(const ecs::obj& child) {
            for (size_t i = 0; i < children_list.length(); ++i) {
                if (children_list[i] == child) {
                    children_list.remove_at(i);
                    break;
                }
            }
        }

        void destroy_hook() {
            for (auto& obj : children_list) {
                obj.destroy();
            }
        }

        array<ecs::obj> children_list;
    };
  
    struct HierarchyView {
        ecs::obj entity;
        
        HierarchyView(const ecs::obj e) : entity(e) {}
        ecs::obj& operator*() {
            return entity;
        }
        const ecs::obj& operator*() const {
            return entity;
        }

        ecs::obj* operator->() {
            return &entity;
        }
        const ecs::obj* operator->() const {
            return &entity;
        }
       
        bool has_children() {
            Parent* prnt = entity.get_component_ptr<Parent>();
            return prnt && prnt->child_count() != 0;
        }
        bool has_parent() {
            return entity.has_component<Child>();
        }
        stn::Option<HierarchyView> parent() {
            Child* res = entity.get_component_ptr<Child>();
            if (res)
            {
                return HierarchyView(res->owner);
            }
            return stn::None;
        }
        array<HierarchyView> children() {
            if (!entity.has_component<Parent>()) return {};

            const auto& children_list= entity.get_component<Parent>().children();
            array<HierarchyView> view;
            for (auto& child: children_list)
            {
                view.push(HierarchyView(child));
            }
            return  view;
        }
        array<ecs::obj> children_obj() {
            if (!entity.has_component<Parent>()) return {};

            return array(entity.get_component<Parent>().children());
            
        }
    };
    HierarchyView view(ecs::obj& object) {

        return HierarchyView(object);
    }
  inline   bool has_children(ecs::obj& entity) {
      return view(entity).has_children();
  }
    bool has_parent(ecs::obj& entity) {
        return view(entity).has_parent();
    }
    stn::Option<ecs::obj> parent(ecs::obj entity) {
        return view(entity).parent().map(std::function([](HierarchyView view) {return view.entity; }));
    }
    array<ecs::obj> children(ecs::obj& entity) {
        return view(entity).children_obj();
    }
} 