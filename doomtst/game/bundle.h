#include <tuple>
#include <utility>
#pragma once
template<typename... Components>
struct Bundle {
	std::tuple<...Components> parts;


	template<typename... Args>
	Bundle(Args&&... args) : parts(std::forward<Args>(args)...) {
	
	}
	constexpr size_t length() const {
		return std::tuple_size<std::tuple<Components...>>::value;
	}
	template<template<typename> class Trait>
	constexpr bool all_satisfy() {
		return (Trait<Components>::value && ...); 
	}

};
