#include "bitset.h"
#include "Id.h"
#pragma once
namespace stn {
    //bitmask type
    template<typename T>
    struct ids {
        using id_type = stn::typed_id<T>;
        ids() :id_bitset() {};
        ids(stn::NoneType) :id_bitset() {};
        ids(stn::bitset&& id_list) :id_bitset(std::move(id_list)) {}
        ids(const stn::bitset& id_list) :id_bitset(id_list) {}
        stn::bitset id_bitset;

        bool has(id_type id) const {
            return id_bitset.contains_index(id.id) && id_bitset[id.id];
        }
        bool has(const ids& other) const {
            return (id_bitset.ones_match(id_bitset | other.id_bitset));
        }
        bool has_none(const ids& other) const {
            return (id_bitset & other.id_bitset).empty();
        }
        stn::array<id_type> into_list() const {
            return id_bitset.indices<id_type>();
        }
        bool operator[](id_type id) const {
            return has(id);
        }

        //need to make const
        size_t total_ids() const {
            return id_bitset.popcount();
        }
        ids without(const ids& other) const {
            stn::bitset other_bits = other.id_bitset;
            other_bits.expand(id_bitset.bits);
            return id_bitset & ~other_bits;
        }
        ids without(id_type id) const {
            ids new_id_bitset = ids(*this);
            new_id_bitset.id_bitset.reaching_disable(id.id);
            return new_id_bitset;
        }
        ids with(const ids& other) const {
            return ids(id_bitset & other.id_bitset);
        }
        ids with(id_type id) const {
            ids new_id_bitset = ids(*this);
            new_id_bitset.id_bitset.reaching_enable(id.id);
            return new_id_bitset;
        }
        //repr

        ids flipped(const ids& other) const {
            return ids(id_bitset ^ other.id_bitset);
        }
        ids flipped(id_type id) const {
            ids new_id_bitset = ids(*this);
            new_id_bitset.id_bitset.reaching_flip(id.id);
            return new_id_bitset;
        }
        bool operator!=(const ids& other) const {
            return !(*this == other);
        }
        bool operator==(const ids& other) const {
            return id_bitset.ones_match(other.id_bitset);
        }
    };

}