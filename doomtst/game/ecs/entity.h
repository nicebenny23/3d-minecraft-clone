#pragma once
#include "../../util/Id.h"
namespace ecs {
	
	struct entity {
  		constexpr std::uint16_t generation() const {
			return gen;
		}
		constexpr std::uint32_t id() const {
			return Id;
		}
		entity(std::uint32_t id, std::uint16_t generation) :Id(id), gen(generation) {}
		friend bool operator==(const entity& lhs, const entity& rhs) {
			return lhs.Id== rhs.Id&&lhs.gen==rhs.gen;
		}
		friend bool operator!=(const entity& lhs, const entity& rhs) {
			return !(lhs == rhs);
		}
		explicit operator bool() const noexcept {
			return Id!=0;
		}

		constexpr entity() noexcept:Id(0), gen(0) {};
	private:
		std::uint16_t gen;
		std::uint32_t Id;

	};

}