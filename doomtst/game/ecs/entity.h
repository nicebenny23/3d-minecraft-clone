#pragma once
#include "../../util/Id.h"
namespace ecs {
	struct entity_tag {
	};
	using entity_id = stn::typed_id<entity_tag>;
	struct entity {
  		constexpr std::uint16_t generation() const {
			return gen;
		}
	
		constexpr entity_id id() const {
			return Id;
		}
		entity(entity_id id, std::uint16_t generation) :Id(id), gen(generation) {}
		friend bool operator==(const entity& lhs, const entity& rhs) {
			return lhs.Id== rhs.Id&&lhs.gen==rhs.gen;
		}
		friend bool operator!=(const entity& lhs, const entity& rhs) {
			return !(lhs == rhs);
		}
		explicit operator bool() const noexcept {
			return Id.id!=0;
		}

		constexpr entity() noexcept:Id(0), gen(0) {};
	private:
		std::uint16_t gen;
		entity_id Id;

	};

}