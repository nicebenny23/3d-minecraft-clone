#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include "../../util/robinHood.h"
#include <stdexcept>
#include   "shader.h"
#include "Uniform.h"
#include "../../util/counter.h"
#include "pass.h"
#include "RenderContext.h"
namespace renderer {
	
	struct Material:assets::Asset {
		std::string name;
		phase_handle pass;
		RenderProperties prop;
		array<renderer::UniformRefrence> handles;
		assets::AssetHandle<Shader> shader;
		bool operator==(const Material& other) const = default;
		
		Material(const std::string& name, phase_handle phase_handle, shader_id shade_handle, const RenderProperties& props, const stn::array<renderer::UniformRefrence>& renderer) :
			pass(phase_handle), shader(shade_handle), name(name), prop(props), handles(renderer)
		{
		}
	};

	struct MaterialDescriptor {
		using asset_type = Material;
		MaterialDescriptor(const std::string& material_name,const std::string& phase_name, const std::string& shader_name, const RenderProperties& props,const stn::array<renderer::UniformRefrence>& renderer) {
			elements = renderer;
			name = material_name;
			phase = phase_name;
			Shader = shader_name;
			properties = props;
		}
		bool operator==(const MaterialDescriptor& other) const = default;

		stn::array<renderer::UniformRefrence> elements;
		std::string name;
		std::string phase;
		std::string Shader;
		RenderProperties properties;
	};
}
namespace std {
	template<>
	struct hash<renderer::MaterialDescriptor> {
		size_t operator()(const renderer::MaterialDescriptor& mat) const noexcept {
			return hash<string>{}(mat.name);
		}
	};

}

namespace renderer {
	
	using MaterialHandle = assets::AssetHandle<Material>;
	struct MaterialManager {
		using load_descriptor = MaterialDescriptor;
	public:
		MaterialManager() {
		}
	
		stn::box<Material> load(const MaterialDescriptor& descriptor,assets::Assets& assets) {
			phase_handle phase = assets.from_name<RenderPhase>(descriptor.phase).expect("phase should exist");
			shader_id shader_handle = assets.from_name<Shader>(descriptor.Shader).expect("shader should exist");
			return stn::box<Material>(descriptor.name, phase, shader_handle, descriptor.properties, descriptor.elements);
		}

		static constexpr bool immortal = true;
		void unload(stn::box<Material> mat) {

		}
	};
	
}