#pragma once
#include "gameobject.h"
#include "../util/Option.h"
#include "../util/SparseSet.h"
#include "../util/pipeline.h"
#include <tuple>
namespace Hierarchy {

    struct Parent;
    struct Child : gameobject::component {
        bool is_child_of(const gameobject::obj& other) const {
            return parent_entity == other;
        }

        gameobject::obj parent() const {
            return parent_entity;
        }

    private:
        friend struct Parent;

        explicit Child(gameobject::obj& parent)
            : parent_entity(parent) {
        }

        void destroy_hook() {
            parent_entity.getcomponent<Parent>().remove_child(owner);
        }

        gameobject::obj parent_entity;
    };


    struct Parent : gameobject::component {
        const array<gameobject::obj>& children() const {
            return children_list;
        }

        bool has_child(const gameobject::obj& obj) const {
            return children_list.contains(obj);
        }
        size_t child_count() const {
            return children_list.length;
        }

        explicit Parent(gameobject::obj& child) {
            children_list.push(child);
        }
        void start() {
            add_child(children_list[0]);
        }

        void add_child(gameobject::obj& child) {
            if (child == owner) {
                throw std::logic_error("An object cannot be its own parent.");
            }
            if (children_list.contains(child)) {
                auto& existing = child.getcomponent<Child>();
                if (existing.parent_entity == owner) {
                    return;
                }
                existing.parent_entity.getcomponent<Parent>().remove_child(child);
                existing.parent_entity = owner;
            }
            else {
                child.addcomponent<Child>(owner);
            }
            children_list.push(child);
        }
    private:

        friend struct Child;
        void remove_child(const gameobject::obj& child) {
            for (size_t i = 0; i < children_list.length; ++i) {
                if (children_list[i] == child) {
                    children_list.deleteind(i);
                    break;
                }
            }
        }

        void destroy_hook() {
            for (auto& obj : children_list) {
                obj.destroy();
            }
        }

        array<gameobject::obj> children_list;
    };
  
    struct HierarchyView {
        gameobject::obj entity;
        
        HierarchyView(const gameobject::obj e) : entity(e) {}
        gameobject::obj& operator*() {
            return entity;
        }
        const gameobject::obj& operator*() const {
            return entity;
        }

        gameobject::obj* operator->() {
            return &entity;
        }
        const gameobject::obj* operator->() const {
            return &entity;
        }
       
        bool has_children() {
            Parent* prnt = entity.getcomponentptr<Parent>();
            return prnt && prnt->child_count() != 0;
        }
        bool has_parent() {
            return entity.hascomponent<Child>();
        }
        Opt::Option<HierarchyView> parent() {
            Child* res = entity.getcomponentptr<Child>();
            if (res)
            {
                return HierarchyView(res->owner);
            }
            return Opt::None;
        }
        array<HierarchyView> children() {
            if (!entity.hascomponent<Parent>()) return {};

            const auto& children_list= entity.getcomponent<Parent>().children();
            array<HierarchyView> view;
            for (auto& child: children_list)
            {
                view.push(HierarchyView(child));
            }
            return  view;
        }
        array<gameobject::obj> children_obj() {
            if (!entity.hascomponent<Parent>()) return {};

            return array(entity.getcomponent<Parent>().children());
            
        }
    };
    HierarchyView view(gameobject::obj& object) {

        return HierarchyView(object);
    }
  inline   bool has_children(gameobject::obj& entity) {
      return view(entity).has_children();
  }
    bool has_parent(gameobject::obj& entity) {
        return view(entity).has_parent();
    }
    Opt::Option<gameobject::obj> parent(gameobject::obj entity) {
        return view(entity).parent().map(std::function([](HierarchyView view) {return view.entity; }));
    }
    array<gameobject::obj> children(gameobject::obj& entity) {
        return view(entity).children_obj();
    }
}