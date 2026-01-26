#pragma once
#include "ecs.h"
#include "../../util/Option.h"
#include "../../util/SparseSet.h"
#include "../../util/pipeline.h"
#include <tuple>
#include "../../util/counter.h"
namespace ecs {



	template<typename T>
	concept RelationshipAccessor =
		std::is_default_constructible_v<T>&&
		std::ranges::forward_range<T>&&
		std::same_as<std::ranges::range_value_t<T>, ecs::entity>&&
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

		using parent_type = Relationship<T, Tag>;
	private:

		friend struct Relationship<T, Tag>;
		inline void destroy_hook();
		entity parent_entity;
	};
	template<RelationshipAccessor T, typename Tag>
	struct Relationship : ecs::component {
		using Accessor = T;
		using TagType = Tag;
		using iterator = typename T::iterator;
		using const_iterator = typename T::const_iterator;
		using child_type = Target<T, Tag>;
		stn::span<ecs::entity> span() requires stn::Spanable<T> {
			return stn::span<ecs::entity>(children_list);
		}
		stn::span<const ecs::entity> span() const requires stn::Spanable<T> {
			return stn::span<const ecs::entity>(children_list);
		}
		bool empty() const {
			return begin() == end();
		}
		iterator begin() {
			return children_list.begin();
		}
		iterator end() {
			return children_list.end();
		}
		const_iterator begin() const {
			return children_list.begin();
		}
		const_iterator end() const {
			return children_list.end();
		}
		bool has_child(const ecs::entity obj) const {
			for (ecs::entity element : children_list) {
				if (element == obj) {
					return true;
				}
			}
			return false;
		}

		explicit Relationship() {
		}
		void start() {
			if constexpr(std::constructible_from<T,Ecs&>) {
				children_list=T(world());
			}
		}
		void swap_children(Relationship& other) requires std::swappable<T> {
			for (ecs::entity child : children_list) {
				world().get_component<child_type>(child).parent_entity = other.owning_entity();
			}
			for (ecs::entity child : other.children_list) {
				world().get_component<child_type>(child).parent_entity = owning_entity();
			}
			std::swap(children_list, other.children_list);

		}

		void add_child(ecs::entity child) {
			if (child == owning_entity()) {
				throw std::logic_error("An object cannot be its own parent.");
			}
			stn::Option<child_type&> child_component = world().get_component_opt<child_type>(child);
			if (child_component) {
				ecs::entity& child_parent = child_component.unwrap().parent_entity;
				world().get_component<Relationship<T, Tag>>(child_parent).remove_child(child);
				child_parent = owning_entity();
			}
			else {
				world().add_component<child_type>(child, owning_entity());
			}
			children_list.add(child);
		}

		void destroy_hook() override {
			for (auto& obj : children_list) {
				world().remove_object_unchecked(obj);
			}
		}
	private:

		friend struct Target<T, Tag>;
		void remove_child(ecs::entity child) {
			children_list.remove(child);
		}


		T children_list;
	};

	template<RelationshipAccessor T, typename Tag>

	void Target<T, Tag>::destroy_hook() {
		world().get_component<parent_type>(parent_entity).remove_child(owning_entity());
	}

}