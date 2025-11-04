#pragma once
#include "../../util/Id.h"
namespace ecs {
	
	struct space_id {
  		constexpr std::uint16_t generation() const {
			return gen;
		}
		constexpr std::uint32_t id() const {
			return Id;
		}
		space_id(std::uint32_t id, std::uint16_t generation) :Id(id), gen(generation) {}
		friend bool operator==(const space_id& lhs, const space_id& rhs) {
			return lhs.Id== rhs.Id&&lhs.gen==rhs.gen;
		}
		friend bool operator!=(const space_id& lhs, const space_id& rhs) {
			return !(lhs == rhs);
		}
		explicit operator bool() const noexcept {
			return Id!=0;
		}

		constexpr space_id() noexcept:Id(0), gen(0) {};
	private:
		std::uint16_t gen;
		std::uint32_t Id;

	};

}