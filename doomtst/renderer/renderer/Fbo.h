#include "GlUtil.h"
#include "../../assets/Asset.h"
#pragma once
namespace renderer {


	struct Fbo {
		Fbo(GLuint fbo_id) :id(fbo_id) {
		}
		GLuint id;
	};

	struct Frame :assets::Asset {
		using asset_type = Fbo;
		bool operator==(const Frame& other) const = default;
		v2::Coord2 size;
		stn::array<FboTextureDescriptor> descriptors;
		std::string name;
		size_t samples;
		Frame(v2::Coord2 frame_size, std::initializer_list<FboTextureDescriptor> descs, const std::string& name, size_t frame_samples = 1)
			: size(frame_size), descriptors(descs), name(name), samples(frame_samples) {
		}
	};
}

namespace std {
	template<>
	struct hash<renderer::Frame> {
		std::size_t operator()(const renderer::Frame& s) const noexcept {
			return std::hash<std::string>()(s.name);
		}
	};
}