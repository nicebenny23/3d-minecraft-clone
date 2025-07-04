#pragma once
#include "mathutil.h"
#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include "dynamicarray.h"

inline size_t words_to_bits(size_t bits) {
    return (bits + 63) / 64;
}

inline size_t calc_leftover_bits(size_t bits) {
    return bits & 63;
}

inline size_t calc_full_words(size_t bits) {
    return bits / 64;
}

//excracts those ovverident during left shift
inline uint64_t extract_high(uint64_t val, size_t n) {
    return val >> (64 - n);
}

//excracts those ovverident during right shift
inline uint64_t extract_low(uint64_t val, size_t n) {
    return val & ((1ULL << n) - 1);
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

    Cont::array<uint64> bitlist;
    size_t bits;

    bitset() = default;

    bitset(const bitset& oth) : bitlist(oth.bitlist), bits(oth.bits) {}

    bitset(bitset&& oth) noexcept : bitlist(std::move(oth.bitlist)), bits(oth.bits) {}

    bitset& operator=(const bitset& oth) {
        if (this != &oth) {
            bitlist = oth.bitlist;
            bits = oth.bits;
        }
        return *this;
    }

    bitset& operator=(bitset&& oth) noexcept {
        if (this != &oth) {
            bitlist = std::move(oth.bitlist);
            bits = oth.bits;
        }
        return *this;
    }

    void destroy() {
        bitlist.destroy();
        bits = 0;
    }

    void set(size_t bit) {
        if (bit >= bits) throw std::out_of_range("Bit index out of range");
        size_t idx = calc_full_words(bit);
        size_t off = calc_leftover_bits(bit);
        bitlist[idx] |= (uint64(1) << off);
    }

    void reset(size_t bit) {
        if (bit >= bits) throw std::out_of_range("Bit index out of range");
        size_t idx = calc_full_words(bit);
        size_t off = calc_leftover_bits(bit);
        bitlist[idx] &= ~(uint64(1) << off);
    }

    bool operator[](size_t bit) const {
        if (bit >= bits) throw std::out_of_range("Bit index out of range");
        size_t word_index = calc_full_words(bit);
        size_t bit_offset = calc_leftover_bits(bit);
        return (bitlist[word_index] >> bit_offset) & 1;
    }

    bitset operator~() const {
        bitset result;
        result.bits = bits;
        result.bitlist.resize(bitlist.length);

        size_t full_words = calc_full_words(bits);
        size_t leftover = calc_leftover_bits(bits);

        for (size_t i = 0; i < full_words; ++i) {
            result.bitlist[i] = ~bitlist[i];
        }

        if (full_words < bitlist.length) {
            uint64_t mask = make_mask(leftover);
            result.bitlist[full_words] = bitlist[full_words] ^ mask;
        }

        return result;
    }

    bitset operator&(const bitset& oth) const {
        size_t min_size = Min(bitlist.length, oth.bitlist.length);
        bitset result;
        result.bitlist.resize(min_size);
        result.bits = Min(bits, oth.bits);
        for (size_t i = 0; i < min_size; i++) {
            result.bitlist[i] = bitlist[i] & oth.bitlist[i];
        }
        return result;
    }

    bitset& operator&=(const bitset& oth) {
        size_t min_size = Min(bitlist.length, oth.bitlist.length);
        bitlist.resize(min_size);
        bits = Min(bits, oth.bits);
        for (size_t i = 0; i < min_size; i++) {
            bitlist[i] &= oth.bitlist[i];
        }
        return *this;
    }

    bitset operator|(const bitset& oth) const {
        size_t max_size = Max(bitlist.length, oth.bitlist.length);
        size_t min_size = Min(bitlist.length, oth.bitlist.length);
        bitset result;
        result.bitlist.resize(max_size);
        result.bits = Max(bits, oth.bits);
        for (size_t i = 0; i < min_size; i++) {
            result.bitlist[i] = bitlist[i] | oth.bitlist[i];
        }
        for (size_t i = min_size; i < max_size; i++) {
            result.bitlist[i] = (bitlist.length == max_size) ? bitlist[i] : oth.bitlist[i];
        }
        return result;
    }

    bitset& operator|=(const bitset& oth) {
        size_t min_size = Min(bitlist.length, oth.bitlist.length);
        for (size_t i = 0; i < min_size; i++) {
            bitlist[i] |= oth.bitlist[i];
        }
        if (oth.bitlist.length > bitlist.length) {
            bitlist.resize(oth.bitlist.length);
            for (size_t i = min_size; i < oth.bitlist.length; i++) {
                bitlist[i] = oth.bitlist[i];
            }
        }
        bits = Max(bits, oth.bits);
        return *this;
    }

    bitset operator^(const bitset& oth) const {
        size_t max_size = Max(bitlist.length, oth.bitlist.length);
        size_t min_size = Min(bitlist.length, oth.bitlist.length);
        bitset result;
        result.bitlist.resize(max_size);
        for (size_t i = 0; i < min_size; i++) {
            result.bitlist[i] = bitlist[i] ^ oth.bitlist[i];
        }
        for (size_t i = min_size; i < max_size; i++) {
            result.bitlist[i] = (bitlist.length == max_size) ? bitlist[i] : oth.bitlist[i];
        }
        result.bits = Max(bits, oth.bits);
        return result;
    }

    bitset& operator^=(const bitset& oth) {
        size_t min_size = Min(bitlist.length, oth.bitlist.length);
        for (size_t i = 0; i < min_size; i++) {
            bitlist[i] ^= oth.bitlist[i];
        }
        if (oth.bitlist.length > bitlist.length) {
            bitlist.resize(oth.bitlist.length);
            for (size_t i = min_size; i < oth.bitlist.length; i++) {
                bitlist[i] = oth.bitlist[i];
            }
        }
        bits = Max(bits, oth.bits);
        return *this;
    }

    bitset operator<<(size_t shift) const {
        if (shift == 0 || bits == 0) return *this;

        bitset result;
        result.bits = bits + shift;
        size_t new_words = words_to_bits(result.bits);
        result.bitlist.resize(new_words);

        size_t old_words = bitlist.length;
        size_t word_shift = calc_full_words(shift);
        size_t bit_shift = calc_leftover_bits(shift);

        for (size_t i = 0; i < old_words; i++) {
            result.bitlist[i + word_shift] = bitlist[i];
        }

        if (bit_shift) {
            uint64_t carry = 0;

            for (size_t i = word_shift; i < new_words; ++i) {
                uint64_t val = result.bitlist[i];

                // Shift left and add in bits carried from the previous word
                result.bitlist[i] = (val << bit_shift) | carry;

                // Save unused bits to carry into the next word
                carry = extract_high(val, bit_shift);
            }
        }

        result.mask_end();
        return result;
    }

    //dont reall understand this
    bitset operator>>(size_t shift) const {
        if (shift == 0 || bits == 0) return *this;
        if (shift >= bits) return bitset{};

        bitset result;
        result.bits = bits - shift;
        result.bitlist.resize(bitlist.length);

        size_t word_shift = calc_full_words(shift);
        size_t bit_shift = calc_leftover_bits(shift);
        size_t len = bitlist.length;

        for (size_t i = 0; i < len; i++) {
            uint64_t value = 0;

            // If the shifted source word is valid
            if (i + word_shift < len) {
                // Shift right by bit offset
                value = bitlist[i + word_shift] >> bit_shift;

                // Carry bits from the next word if possable
                if (bit_shift > 0 && i + word_shift + 1 < len) {

                    value |= extract_low(bitlist[i + word_shift + 1], bit_shift) << (64 - bit_shift);
                }
            }

            result.bitlist[i] = value;
        }

        result.mask_end();
        return result;
    }

    bitset& push(const bitset& oth) {
        *this |= oth << bits;
        bits += oth.bits;
        return *this;
    }

    bitset& push(bool oth) {
        size_t idx = calc_full_words(bits);
        size_t off = calc_leftover_bits(bits);

        if (idx >= bitlist.length) {
            bitlist.push(0);
        }
        if (oth) {
            bitlist[idx] ^= bit_at(off);
        }
        ++bits;
        return *this;
    }
    bool operator==(const bitset& oth) const {
        return bits == oth.bits && bitlist == oth.bitlist;
    }

    bool operator!=(const bitset& oth) const {
        return !(*this == oth);
    }
private:
    //trims the unused bits
    void mask_end() {
        size_t leftover = calc_leftover_bits(bits);
        if (leftover != 0 && bitlist.length > 0) {
            bitlist[bitlist.length - 1] &= make_mask(leftover);
        }
    }
};
