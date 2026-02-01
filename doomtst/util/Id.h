#pragma once
#include "exception.h"
#include <stdint.h>
#include "data_format.h"
#include <limits>
namespace stn {
	static constexpr uint32_t none_id = std::numeric_limits<uint32_t>::max();

	
	struct Id {
		uint32_t id;
		explicit constexpr Id():id(none_id){

		}
		constexpr explicit Id(uint32_t val) : id(val) {}
		constexpr bool valid() const noexcept{ return id != none_id; }
		constexpr bool operator==(const Id& other) const { return id == other.id; }
		constexpr bool operator!=(const Id& other) const { return id != other.id; }
		constexpr explicit operator bool() const { return valid(); }
	};

	template<typename T>
	struct typed_id {
		uint32_t id;
		constexpr explicit typed_id(uint32_t val) : id(val) {}
		constexpr bool operator==(const typed_id& other) const { return id == other.id; }
		constexpr bool operator!=(const typed_id& other) const { return id != other.id; }
		typed_id() = delete;
		constexpr explicit operator uint32_t() const { return id; }
		constexpr explicit operator size_t() const { return static_cast<size_t>(id); }
	};
	//nonmovable id type
	template<typename T>
	struct token_id {

		uint32_t id;
		constexpr explicit token_id(uint32_t val) : id(val) {}
		constexpr bool operator==(const token_id& other) const { return id == other.id; }
		constexpr bool operator!=(const token_id& other) const { return id != other.id; }
		token_id() = delete;
		token_id(token_id&& other) :id(other.id) {};
		token_id(const token_id& other) = delete;
		token_id& operator=(const token_id& other) = delete;
		token_id& operator=(token_id&& other) { id = other.id; return *this; };

		constexpr explicit operator uint32_t() const { return id; }
		constexpr explicit operator size_t() const { return static_cast<size_t>(id); }
	};
	//special option type for an id
	template<typename T>
	struct default_id {
	
		constexpr explicit default_id(uint32_t val) : id(val) {}
		constexpr bool operator==(const default_id& other) const { return id == other.id; }
		constexpr bool operator!=(const default_id& other) const { return id != other.id; }
		constexpr default_id() :id(none_id) {};
		constexpr default_id(const typed_id<T>& ty_id) :id(ty_id.id){ };
		explicit operator bool() {
			return bounded();
		}
		constexpr bool bounded() const noexcept { return id != none_id; }
		constexpr bool unbounded() const noexcept { return id == none_id; }
		typed_id<T> as_id() const { 
			if (unbounded())
			{
				throw std::logic_error("an unbounded block_id cannot be transformed into an block_id");
			}
			return typed_id<T>(id);
		}
		 uint32_t get() const {
			if (unbounded())
			{
				throw std::logic_error("an unbounded block_id has no value");
			}
			return id;
		}
		constexpr uint32_t get_unchecked() const {
			return id;
		}
		void assert_bounded(const char* msg) const {
			if (unbounded())
			{
				throw std::logic_error(msg);
			}
		}
		constexpr void reset() {
			id = none_id;
		}
		
	private:
		uint32_t id;
	};

}
#include <functional>
template<typename T>
struct std::hash<stn::typed_id<T>> {
	size_t operator()(stn::typed_id<T> t) const noexcept {
		return std::hash<uint32_t>{}(t.id);
	}
};

template <typename T>
struct std::formatter<stn::typed_id<T>> : std::formatter<std::string> {
	template <typename FormatContext>
	auto format(const stn::typed_id<T>& v, FormatContext& ctx) const {
		return std::format_to(ctx.out(), "{}", v.id);
	}
};
