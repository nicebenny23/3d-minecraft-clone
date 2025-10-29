#include <type_traits>
#include <algorithm>
#pragma once
namespace stn::memory {
	struct layout {
		constexpr layout(size_t size_of, size_t align_of) noexcept :size(size_of), alignment(align_of) {};
		constexpr size_t align_up(size_t offset) const {
			return (offset + alignment - 1) & ~(alignment - 1);
		}
		size_t size;
		size_t alignment;
		constexpr bool fits(const layout& other) const {
			return (other.size <= size) && (alignment % other.alignment == 0);
		}
	};
	template<typename T>
	constexpr layout get_layout() {
		if constexpr (std::is_reference_v<T>) {
			using ptr = std::add_pointer_t<std::remove_reference_t<T>>;
			return get_layout<ptr>(); // just use pointer layout
		}
		else {
			return layout( sizeof(T), alignof(T) );
		}
	}
	//variant
	template<typename ...Args>
	constexpr layout overlay(const Args&... layouts) {
		size_t max_size = (std::max)({ static_cast<size_t>(layouts.size)... }); // same effect
		size_t max_align = (std::max)({ static_cast<size_t>(layouts.alignment)... });
		return layout(max_size, max_align);
	}
	//struct
	template<typename... Args>
	constexpr layout join(const Args&... layouts) noexcept {
		size_t total_size = 0;
		((total_size += layouts.size), ...); // proper fold expression
	size_t max_align = (std::max)({ static_cast<size_t>(layouts.alignment)... });
		return layout(total_size, max_align);
	}

	template<typename T>
	constexpr layout layout_of = get_layout<T>();
	template<typename ...Ty>
	constexpr layout overlay_of= overlay(layout_of<Ty>...);
	template<typename ...Ty>
	constexpr layout join_of=join(layout_of<Ty>...);

		//struct like composition
	

}