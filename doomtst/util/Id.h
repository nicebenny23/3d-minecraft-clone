#pragma once
#include <stdint.h>
#include <limits>
namespace Ids {
	static constexpr uint32_t null_id = std::numeric_limits<uint32_t>::max();
	struct Id {
	
		uint32_t id;
		uint32_t gen;
		constexpr Id() noexcept {

			id = null_id;
			gen = 0;
		}
		explicit operator bool() const noexcept {
			return id != null_id;
		}
		bool operator==(const Id& oth) const noexcept {
			return id == oth.id && gen == oth.gen;
		}
		bool operator!=(const Id& oth) const noexcept {
			return !(*this == oth);
		}
		constexpr Id(uint32_t id, uint32_t gen) noexcept :id(id), gen(gen) {};

	};
	static constexpr Id None = Id(null_id,0);
	struct IdFlattener{
		size_t operator()(const Id& id) const noexcept {
			return id.id;
		}
	};

	struct IdHash{
		size_t operator()(const Id& id) const noexcept {

			return std::hash<size_t>{}((size_t(id.gen)<<32)+id.id);
		}
	};


}