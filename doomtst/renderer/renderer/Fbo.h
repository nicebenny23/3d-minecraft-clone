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

	struct FrameLoader {

		stn::box<Fbo> Load(const Frame& frame, assets::Assets& assets) {
			Fbo fbo(0);
			glGenFramebuffers(1, &fbo.id);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
			int colorAttachmentIndex = 0;
			std::vector<GLenum> drawBuffers;
			GLenum texTarget = (frame.samples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

			for (const FboTextureDescriptor& desc : frame.descriptors) {
				assets::AssetHandle<Texture2D> tex = assets.load<FboTextureDescriptor>(desc).expect("asset should exist");

				GLenum attachment;
				switch (desc.type) {
					case FboTextureDescriptor::Type::Color:
					case FboTextureDescriptor::Type::Integer:
					attachment = GL_COLOR_ATTACHMENT0 + colorAttachmentIndex++;
					drawBuffers.push_back(attachment);
				break;

					case FboTextureDescriptor::Type::Depth:
					attachment = GL_DEPTH_ATTACHMENT;
					break;
				}
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texTarget, tex->id, 0);
			}
			if (!drawBuffers.empty()) {
				glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
			}
			else {
				glDrawBuffer(GL_NONE);
			}
			
		}
		static constexpr bool immortal = true;
		void unload(stn::box<Frame>&& frame) {
			//idk;
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