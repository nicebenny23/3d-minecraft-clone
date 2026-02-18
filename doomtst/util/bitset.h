#pragma once
#include <cstdint>
#include <stdexcept>
#include "../math/mathutil.h"
#include <algorithm>
#include "dynamicarray.h"
#include <bit>
namespace stn {

	inline size_t bits_to_words(size_t bits) {
		return (bits + 63) / 64;
	}

	inline size_t calc_leftover_bits(size_t bits) {
		return bits & 63;
	}

	inline size_t calc_full_words(size_t bits) {
		return bits / 64;
	}

	inline uint64_t make_mask(size_t n) {
		return (n == 64) ? ~0ULL : ((1ULL << n) - 1);
	}
	//uint64_t with bit at nth positon
	inline constexpr uint64_t bit_at(size_t offset) {
		return uint64_t(1) << offset;
	}
	struct bitset {
		using uint64 = uint64_t;
		stn::array<uint64> bit_list;
		size_t bits;
		inline bool contains_index(size_t index) const {
			return index < bits;
		}
		inline bool contains(size_t index) const {
			return index < bits && unchecked_at(index);
		}
		bitset(size_t initial_zeros) noexcept :bits(initial_zeros) {
			bit_list.expand(bits_to_words(initial_zeros));
		};

		constexpr bitset() noexcept :bits(0), bit_list() {
		};

		template<typename T>
		bitset(const stn::array<T>& array) {
			size_t max = 0;
			bits = 0;
			for (size_t i = 0; i < array.length(); i++) {
				max = stn::max(static_cast<size_t>(array[i]), max);
			}
			expand(max + 1);
			for (size_t i = 0; i < array.length(); i++) {

				enable(static_cast<size_t>(array[i]));
			}
		}

		bitset(const bitset& oth) : bit_list(oth.bit_list), bits(oth.bits) {
		}

		bitset(bitset&& oth) noexcept : bit_list(std::move(oth.bit_list)), bits(oth.bits) {
			oth.bits = 0;
		}

		bitset& operator=(const bitset& oth) {
			if (this != &oth) {
				bit_list = oth.bit_list;
				bits = oth.bits;
			}
			return *this;
		}
		bitset(size_t length, bool state) {
			bits = length;
			expand(length);
			size_t numwords = bits_to_words(length);
			uint64_t fill = state ? ~uint64_t(0) : 0;
			for (size_t i = 0; i < numwords; ++i) {
				bit_list[i] = fill;
			}
			mask_end();
		}
		bool has_all_ones(const bitset& oth) const noexcept {
			size_t words_in_all = std::min(oth.bit_list.length(), bit_list.length());
			for (size_t i = 0; i < words_in_all; i++) {
				//we can safely do this due to our invarient that the ends are masked
				if (oth.bit_list[i] & ~bit_list[i]) {
					return false;
				}
			}
			for (size_t i = words_in_all; i < oth.bit_list.length(); i++) {
				if (oth.bit_list[i]) {
					return false;
				}
			}
			return true;
		}
		bitset& operator=(bitset&& oth) noexcept {
			if (this != &oth) {
				bit_list = std::move(oth.bit_list);
				bits = oth.bits;
			}
			return *this;
		}
		template<typename T>
		stn::array<T> indices() const {
			stn::array<T> inds = stn::array<T>();
			for (std::uint32_t i = 0; i < bits; i++) {
				if ((*this)[i]) {
					inds.emplace(i);
				}
			}
			return inds;
		}
		~bitset() {

		}
		//zero initilizes and expands
		void expand(size_t new_bit_count) {
			if (bits < new_bit_count) {
				bits = new_bit_count;
				bit_list.expand(bits_to_words(new_bit_count));
			}
		}
		void set_unchecked(size_t bit, bool value) {
			size_t idx = calc_full_words(bit);
			size_t off = calc_leftover_bits(bit);
			if (value) {
				bit_list.unchecked_at(idx) |= bit_at(off);
			}
			else {
				bit_list.unchecked_at(idx) &= ~(bit_at(off));
			}
		}
		void set(size_t bit, bool value) {
			if (!contains_index(bit)) {
				stn::throw_range_exception("set failed: index {} out of bounds (len {})", bit, bits);
			}
			size_t idx = calc_full_words(bit);
			size_t off = calc_leftover_bits(bit);
			if (value) {
				bit_list.unchecked_at(idx) |= bit_at(off);
			}
			else {
				bit_list.unchecked_at(idx) &= ~(bit_at(off));
			}
		}
		void reaching_set(size_t bit, bool value) {

			expand(bit + 1);
			size_t idx = calc_full_words(bit);
			size_t off = calc_leftover_bits(bit);
			if (value) {
				bit_list.unchecked_at(idx) |= bit_at(off);
			}
			else {
				bit_list.unchecked_at(idx) &= ~(bit_at(off));
			}
		}

		void enable(size_t bit) {
			set(bit, true);
		}
		void disable(size_t bit) {
			set(bit, false);
		}
		void reaching_enable(size_t bit) {
			reaching_set(bit, true);
		}
		void reaching_disable(size_t bit) {
			reaching_set(bit, false);
		}
		void enable_unchecked(size_t bit) {
			set_unchecked(bit, true);
		}
		void disable_unchecked(size_t bit) {
			set_unchecked(bit, false);
		}
		void flip(size_t bit) {
			if (!contains_index(bit)) {
				stn::throw_range_exception("flip failed: index {} out of bounds (len {})", bit, bits);
			}
			size_t idx = calc_full_words(bit);
			size_t off = calc_leftover_bits(bit);
			bit_list[idx] ^= (bit_at(off));
		}
		void reaching_flip(size_t bit) {
			if (!contains_index(bit)) {
				expand(bit + 1);
			}
			size_t idx = calc_full_words(bit);
			size_t off = calc_leftover_bits(bit);
			bit_list[idx] ^= (bit_at(off));
		}

		bool unchecked_at(size_t bit) const {
			return (bit_list.unchecked_at(calc_full_words(bit)) >> calc_leftover_bits(bit)) & 1;
		}
		bool operator[](size_t bit) const {
			if (!contains_index(bit)) {
				stn::throw_range_exception("operator[] failed: index {} out of bounds (len {})", bit, bits);
			}
			size_t word_index = calc_full_words(bit);
			size_t bit_offset = calc_leftover_bits(bit);
			return (bit_list[word_index] >> bit_offset) & 1;
		}
		bitset operator|(const bitset& oth) const {
			size_t max_size = max(bit_list.length(), oth.bit_list.length());
			size_t min_size = min(bit_list.length(), oth.bit_list.length());
			bitset result(max(bits, oth.bits));
			for (size_t i = 0; i < min_size; i++) {
				result.bit_list[i] = bit_list[i] | oth.bit_list[i];
			}
			for (size_t i = min_size; i < max_size; i++) {
				result.bit_list[i] = (bit_list.length() == max_size) ? bit_list[i] : oth.bit_list[i];
			}
			return result;
		}
		//upto the maximum 
		bitset& operator|=(const bitset& oth) {
			*this = *this | oth;
			return *this;
		}

		bitset operator~() {
			bitset result(bits);
			size_t full_words = calc_full_words(bits);
			for (size_t i = 0; i < full_words; ++i) {
				result.bit_list[i] = ~bit_list[i];
			}

			result.mask_end();
			return result;
		}
		//
		bitset operator&(const bitset& oth) const {
			size_t min_size = min(bit_list.length(), oth.bit_list.length());
			bitset result(min(bits, oth.bits));
			for (size_t i = 0; i < min_size; i++) {
				result.bit_list[i] = bit_list[i] & oth.bit_list[i];
			}
			return result;
		}

		bitset& operator&=(const bitset& oth) {
			*this = *this & oth;
			return *this;
		}
		void push(bool bit) {
			reaching_set(bits, bit);
		}

		bitset operator^(const bitset& oth) const {
			size_t max_size = max(bit_list.length(), oth.bit_list.length());
			size_t min_size = min(bit_list.length(), oth.bit_list.length());
			bitset result(max(bits, oth.bits));
			for (size_t i = 0; i < min_size; i++) {
				result.bit_list[i] = (bit_list[i] ^ oth.bit_list[i]);
			}
			for (size_t i = min_size; i < max_size; i++) {
				result.bit_list[i] = (bit_list.length() == max_size) ? bit_list[i] : oth.bit_list[i];
			}

			return result;
		}

		bitset& operator^=(const bitset& oth) {
			*this = *this ^ oth;
			return *this;
		}

		bool operator==(const bitset& oth) = delete;
		bool empty() {
			for (std::uint64_t word : bit_list) {
				if (word != 0) {
					return false;
				}
			}
			return true;
		}
		bool operator!=(const bitset& oth) = delete;
		size_t popcount() const {
			size_t cnt = 0;
			for (std::uint64_t word : bit_list) {
				cnt += std::popcount(word);
			}
			return cnt;
		}
		bool ones_match(const bitset& oth) const {
			size_t min_size = min(oth.bit_list.length(), bit_list.length());

			for (size_t i = 0; i < min_size; i++) {
				if (bit_list[i] != oth.bit_list[i]) {
					return false;
				}
			}
			for (size_t i = min_size; i < bit_list.length(); i++) {
				if (bit_list[i] != 0) {
					return false;
				}
			}

			for (size_t i = min_size; i < oth.bit_list.length(); i++) {
				if (oth.bit_list[i] != 0) {
					return false;
				}
			}
			return true;
		}

		size_t matches(const bitset& oth) const {
			return (*this & oth).popcount();
		}
		bool matches_any(const bitset& oth) const {
			return this->matches(oth) != 0;
		}
		void clear() {
			bit_list.clear();
			bits = 0;
		}
	private:
		//trims the unused bits
		void mask_end() {
			size_t leftover = calc_leftover_bits(bits);
			if (leftover != 0 && !bit_list.empty()) {
				bit_list.last() &= make_mask(leftover);
			}
		}

	};

}