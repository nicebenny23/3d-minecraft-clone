#include "relationship.h"
#pragma once
namespace ecs {

	struct ParentArrayAdapter {
		stn::array<ecs::entity> Container;

		using iterator = stn::array<ecs::entity>::iterator;
		using const_iterator = stn::array<ecs::entity>::const_iterator;

		void add(ecs::entity v) {
			Container.push(v);
		}

		void remove(ecs::entity value) {
			for (size_t i = 0; i < Container.length(); ++i) {
				if (Container[i] == value) {
					Container.swap_drop_unchecked(i);
					return;
				}
			}
		}

		iterator begin() {
			return Container.begin();
		}
		iterator end() {
			return Container.end();
		}
		const_iterator begin() const {
			return Container.begin();
		}
		const_iterator end()   const {
			return Container.end();
		}
	};

	struct parent_child_tag {
	};
	using Parent = ecs::Relationship<ParentArrayAdapter, parent_child_tag >;
	using Child = ecs::Target<ParentArrayAdapter, parent_child_tag>;

	
}