#include <tuple>
#pragma once

namespace stn::member_hash {
    
    //using member_hash
    // generic helper: hashes a list of member pointers
    template <auto... Members>
    struct MemberList {
        template <typename T>
        static auto as_tuple(const T& obj) {
            return std::make_tuple(obj.*Members...);
        }
    };
    template <typename>
    struct IsMemberListType : std::false_type {};
    template <auto... Members>
    struct IsMemberListType<MemberList<Members...>> : std::true_type {};
    template <typename T>
    concept HasMemberHash = IsMemberListType<decltype(T::member_hash)>::value;
}
namespace std {
    template <typename T>
        requires stn::member_hash::HasMemberHash<T>
    struct hash<T> {
        std::size_t operator()(const T& obj) const noexcept {
            const auto& tuple = T::member_hash::as_tuple(obj);
            return std::hash<decltype(tuple)>{}(tuple);
        }
    };
}
