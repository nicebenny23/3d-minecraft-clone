#pragma once
#include "../../util/Id.h"
namespace ecs::entity {
	struct entity {
		size_t generation() const {
			return gen;
		}
		size_t id() const {
			return Id;
		}
		entity(size_t id, size_t generation) :Id(id), gen(generation) {}
		friend bool operator==(const entity& lhs, const entity& rhs) {
			return lhs.Id== rhs.Id&&lhs.gen==rhs.gen;
		}
		friend bool operator!=(const entity& lhs, const entity& rhs) {
			return !(lhs == rhs);
		}

	private:
		std::uint16_t gen;
		std::uint32_t Id;

	};

}