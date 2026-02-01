#include "fileloader.h"
#pragma once
namespace stn {
	constexpr uint64_t fnv1a_hash(std::string_view str) {
		uint64_t hash = 14695981039346656037ull;
		for (char element: str) {
			hash ^= static_cast<uint64_t>(element);
			hash *= 1099511628211ull;
		}
		return hash;
	}
	struct HashedString {
	
		constexpr HashedString(std::string_view view):str(view),hash_value(fnv1a_hash(view)){

		}
		constexpr size_t hash() const{
			return hash_value;
		}
		constexpr bool operator==(const HashedString& other) const noexcept {
			return hash_value == other.hash_value && str == other.str;
		}

		constexpr bool operator==(std::string_view other) const noexcept {
			return str == other;
		}
		constexpr operator std::string_view() const noexcept {
			return str;
		}
		constexpr std::string_view view() const noexcept {
			return str;
		}
		constexpr bool operator==(const std::string& other) const noexcept {
			return str == other;
		}
		private:
		std::string_view str;
		size_t hash_value;
	};
	
	struct StringHasher {
		using is_transparent = void;

		constexpr size_t operator()(std::string_view v) const noexcept {
			return fnv1a_hash(v);
		}

		constexpr size_t operator()(const std::string& s) const noexcept {
			return fnv1a_hash(s);
		}
		constexpr size_t operator()(const HashedString& s) const noexcept {
			return s.hash();
		}
	};
}