#pragma once
#include "game_object.h"
#include "../../util/Option.h"
#include "../../util/SparseSet.h"
#include "../../util/pipeline.h"
#include <tuple>
#include "../../util/counter.h"
namespace ecs {



	template<typename T>
	concept RelationshipAccessor =
		std::ranges::forward_range<T> && std::same_as<std::ranges::range_value_t<T>, ecs::entity>&&
		requires(T& r, const T& cr, ecs::entity o) {
			{
				r.add(o)
			} -> std::same_as<void>;
			{
				r.remove(o)
			} -> std::same_as<void>;
			{
				std::ranges::begin(cr)
			} -> std::forward_iterator;
			{
				std::ranges::end(cr)
			} -> std::forward_iterator;
	};

	template<RelationshipAccessor T, typename Tag>
	struct Relationship;
	template<RelationshipAccessor T, typename Tag>
	struct Target : ecs::component {
		
		entity parent() const {
			return parent_entity;
		}
		explicit Target(entity parent)
			: parent_entity(parent) {
		}

		using parent_type =Relationship<T,Tag>;
	private:

		friend struct Relationship<T, Tag>;
		inline void destroy_hook();
		entity parent_entity;
	};
	template<RelationshipAccessor T,typename Tag>
	struct Relationship : ecs::component {
		
		using iterator = typename T::iterator;
		using const_iterator = typename T::const_iterator;
		using child_type = Target<T, Tag>;
		stn::span<ecs::entity> span() requires stn::Spanable<T>{
			return stn::span<ecs::entity>(children_list);
		}
		stn::span<const ecs::entity> span() const requires stn::Spanable<T> {
			return stn::span<const ecs::entity>(children_list);
		}
		bool empty() const{
			return begin() == end();
		}
		iterator begin() {
			return children_list.begin();
		}
		iterator end() {
			return children_list.end();
		}
		const_iterator begin() const{
			return children_list.begin();
		}
		const_iterator end() const{
			return children_list.end();
		}
		bool has_child(const ecs::entity obj) const {
			for (ecs::entity element:children_list) {
				if (element == obj) {
					return true;
				}
			}
			return false;
		}

		explicit Relationship() {
		}
		void start() {
		}
		void swap_children(Relationship& other) requires std::swappable<T>{
			for (ecs::entity child : children_list) {
				world().get_component<child_type>(child).parent_entity = other.owner();
			}
			for (ecs::obj child : other.children_list) {
				child.get_component<child_type>().parent_entity = owner();
			}
			std::swap(children_list,other.children_list);

		}

		void add_child(ecs::obj child) {
			if (child == owner()) {
				throw std::logic_error("An object cannot be its own parent.");
			}
			stn::Option<child_type&> child_component = child.get_component_opt<child_type>();
			if (child_component) {
				ecs::entity& child_parent= child_component.unwrap().parent_entity;
				world().get_component<Relationship<T, Tag>>(child_parent).remove_child(child);
				child_parent =owner().inner();
			}
			else {
				child.add_component<child_type>(owner().inner());
			}
			children_list.add(child.inner());
		}

	private:
		
		friend struct Target<T,Tag>;
		void remove_child(const ecs::obj child) {
			children_list.remove(child.inner());
		}

		void destroy_hook() {
			for (auto& obj : children_list) {
				ecs::obj(obj,world()).destroy();
			}
		}

		T children_list;
	};
	
	struct ParentArrayAdapter {
		stn::array<ecs::entity> container;

		using iterator = stn::array<ecs::entity>::iterator;
		using const_iterator = stn::array<ecs::entity>::const_iterator;

		void add(ecs::entity v) {
			container.push(v);
		}

		void remove(ecs::entity value) {
			for (size_t i = 0; i < container.length(); ++i) {
				if (container[i] == value) {
					container.swap_drop_unchecked(i);
					return;
				}
			}
		}

		iterator begin() { return container.begin(); }
		iterator end()   { return container.end(); }
		const_iterator begin() const { return container.begin(); }
		const_iterator end()   const { return container.end(); }
	};
	struct parent_child_tag {
	};
	using Parent = ecs::Relationship<ParentArrayAdapter, parent_child_tag >;
	using Child = ecs::Target<ParentArrayAdapter, parent_child_tag>;

	inline void add_child(ecs::obj parent, ecs::obj child) {
		parent.ensure_component<Parent>().add_child(child);
	}
	struct HierarchyView {
		ecs::obj entity;

		HierarchyView(const ecs::obj e) : entity(e) {
		}
		ecs::obj operator*() {
			return entity;
		}
		const ecs::obj operator*() const {
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
			return prnt && !prnt->empty();
		}
		bool has_parent() {
			return entity.has_component<Child>();
		}
		stn::Option<HierarchyView> parent() {
			return entity.get_component_opt<Child>().map([&](Child& child) {return HierarchyView(ecs::obj(child.parent(), child.world())); });
		}
		stn::array<HierarchyView> children() {
			if (!entity.has_component<Parent>()) return {};

			stn::array<HierarchyView> view;
			for (const auto& child : entity.get_component<Parent>().span()) {
				view.push(HierarchyView(ecs::obj(child,entity.world())));
			}
			return view;
		}
		stn::span<ecs::entity> children_obj() {
			if (!entity.has_component<Parent>()) return {};

			return entity.get_component<Parent>().span();

		}
	};
	template<RelationshipAccessor T,typename Tag>
	void Target<T, Tag>::destroy_hook() {
		world().get_component<parent_type>(parent_entity).remove_child(owner());
	}
	inline HierarchyView view(ecs::obj object) {

		return HierarchyView(object);
	}
	inline bool has_children(ecs::obj entity) {
		return view(entity).has_children();
	}
	inline bool has_parent(ecs::obj entity) {
		return view(entity).has_parent();
	}
	inline stn::Option<ecs::obj> parent(ecs::obj entity) {
		return view(entity).parent().map(std::function([](HierarchyView view) {return view.entity; }));
	}
	inline stn::span<ecs::entity> children(ecs::obj entity) {
		return view(entity).children_obj();
	}
	inline void make_parent_of(ecs::obj parent, ecs::obj child) {
		parent.ensure_component<Parent>().add_child(child);
	}
	//should make recip
	template<RecipeType T, typename ...Args> requires std::constructible_from<T, Args&&...>
	ecs::obj spawn_child(ecs::obj parent, Args&&... args) {
		ecs::obj child=ecs::spawn_emplaced<T>(parent.world(), std::forward<Args>());
		make_parent_of(parent, child);
		return child;
	}
}