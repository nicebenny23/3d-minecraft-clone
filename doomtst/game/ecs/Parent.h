#include "relationship.h"
#pragma once
namespace ecs {

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

		iterator begin() {
			return container.begin();
		}
		iterator end() {
			return container.end();
		}
		const_iterator begin() const {
			return container.begin();
		}
		const_iterator end()   const {
			return container.end();
		}
	};

	struct parent_child_tag {
	};
	using Parent = ecs::Relationship<ParentArrayAdapter, parent_child_tag >;
	using Child = ecs::Target<ParentArrayAdapter, parent_child_tag>;

	//should make recip
	
}