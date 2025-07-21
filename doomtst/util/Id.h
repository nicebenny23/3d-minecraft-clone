#pragma once
#include <stdint.h>
#include <limits>
namespace Ids {
	static constexpr uint32_t none_id = std::numeric_limits<uint32_t>::max();
	struct GenId {
	
		uint32_t id;
		uint32_t gen;
		constexpr GenId() noexcept {

			id = none_id;
			gen = 0;
		}
		explicit operator bool() const noexcept {
			return id != none_id;
		}
		bool operator==(const GenId& oth) const noexcept {
			return id == oth.id && gen == oth.gen;
		}
		bool operator!=(const GenId& oth) const noexcept {
			return !(*this == oth);
		}
		constexpr GenId(uint32_t id, uint32_t gen) noexcept :id(id), gen(gen) {};

	};
	static constexpr GenId NoneG = GenId(none_id,0);
	struct IdFlattener{
		size_t operator()(const GenId& id) const noexcept {
			return id.id;
		}
	};

	struct IdHash{
		size_t operator()(const GenId& id) const noexcept {

			return std::hash<size_t>{}((size_t(id.gen)<<32)+id.id);
		}
	};



	
	struct Id {
		uint32_t value;

		constexpr explicit Id(uint32_t val = none_id) : value(val) {}

		constexpr bool inline valid() const { return value != none_id; }
		constexpr bool operator==(const Id& other) const { return value == other.value; }
		constexpr bool operator!=(const Id& other) const { return value != other.value; }
		constexpr explicit operator bool() const { return valid(); }
	};

	inline constexpr Id None{ none_id };

}