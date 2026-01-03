#pragma once
#include <concepts>
#include <string_view>
namespace stn {
	template<typename T>
	concept Namable = requires(const T & t) {
		{
			t.name()
		} -> std::convertible_to<std::string_view>;
	} || requires(const T & t) {
		{
			t.name
		}   -> std::convertible_to<std::string_view>;
	};
	template<Namable T>
	std::string_view name(const T& object) {
		if constexpr (requires { object.name(); }) {
			return object.name();
		}
		else {
			return object.name;
		}
	}
	template<Namable A, Namable B>
	bool same_names(const A& a, const B& b) {
		return name(a) == name(b);
	}

}