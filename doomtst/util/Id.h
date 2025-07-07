#pragma once
#include <stdint.h>
namespace Ids {
	struct Id {

		uint32_t id;
		uint32_t gen;
		constexpr Id() noexcept {

			id = 0;
			gen = 0;
		}
		explicit operator bool() const noexcept {
			return id != 0;
		}
		bool operator == (const Id& oth) const{

			return id == oth.id && oth.gen == gen && *this;
		}
		bool operator !=(const Id& oth) const {

			return id != oth.id || oth.gen != gen || !*this;
		}
		constexpr Id(uint32_t id, uint32_t gen) noexcept :id(id), gen(gen) {};

	};
	static constexpr Id None = Id();
}