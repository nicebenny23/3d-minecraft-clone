
#pragma once
#include <string>
#include <functional>
#include "../../assets/Asset.h"
namespace renderer{ 

	struct render_phase : public assets::Asset {
		size_t priority;
		bool should_sort;
		std::string name;

		render_phase(size_t priority, bool should_sort, const std::string& name)
			: priority(priority), should_sort(should_sort), name(name) {
		}

		bool operator==(const render_phase& other) const {
			return priority == other.priority
				&& should_sort == other.should_sort
				&& name == other.name;
		}

		bool operator!=(const render_phase& other) const {
			return !(*this == other);
		}
			using asset_type = render_phase;
	};

	using phase_handle= assets::AssetHandle<render_phase>;
}

namespace std {
	template<>
	struct hash<renderer::render_phase> {
		std::size_t operator()(const renderer::render_phase& phase) const noexcept {
			std::size_t h1 = std::hash<size_t>{}(phase.priority);
			std::size_t h2 = std::hash<bool>{}(phase.should_sort);
			std::size_t h3 = std::hash<std::string>{}(phase.name);
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};
}
