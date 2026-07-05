
#pragma once
#include <string>
#include <functional>
#include "../../assets/Asset.h"
namespace renderer{ 

	struct RenderPhase : public assets::Asset {
		size_t priority;
		bool should_sort;
		std::string name;

		RenderPhase(size_t priority, bool should_sort, const std::string& name)
			: priority(priority), should_sort(should_sort), name(name) {
		}

		bool operator==(const RenderPhase& other) const {
			return priority == other.priority
				&& should_sort == other.should_sort
				&& name == other.name;
		}

		bool operator!=(const RenderPhase& other) const {
			return !(*this == other);
		}
			using asset_type = RenderPhase;
	};

	using phase_handle= assets::AssetHandle<RenderPhase>;
}

namespace std {
	template<>
	struct hash<renderer::RenderPhase> {
		std::size_t operator()(const renderer::RenderPhase& phase) const noexcept {
			std::size_t h1 = std::hash<size_t>{}(phase.priority);
			std::size_t h2 = std::hash<bool>{}(phase.should_sort);
			std::size_t h3 = std::hash<std::string>{}(phase.name);
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};
}
