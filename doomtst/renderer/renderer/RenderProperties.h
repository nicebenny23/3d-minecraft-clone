#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include "../../util/robinHood.h"
#include <stdexcept>
#include   "shader.h"
#include "Uniform.h"
#include "ShaderManager.h"
#include "../../util/counter.h"
#include "pass.h"
#include "RenderContext.h"
namespace renderer {
	struct  Renderer;


	struct material_tag {
	};
	struct Material:assets::Asset {
		std::string name;

		phase_handle pass;
		RenderProperties prop;
		
		array<uniforms::uniform_ref> handles;
		assets::AssetHandle<shader> shader;
		bool operator==(const Material& other) const = default;
		
		Material(const std::string& name, phase_handle phase_handle, renderer::shader_id shade_handle, const RenderProperties& props = {}, const stn::array<uniforms::uniform_ref>& uniforms) :
			pass(phase_handle), shader(shade_handle), name(name), prop(props), handles(uniforms)
		{
		}
	};

	struct MaterialDescriptor {
		using asset_type = Material;
		MaterialDescriptor(std::string& material_name, std::string phase_name, std::string shader_name, const RenderProperties& props = {}, std::initializer_list<stn::pair<const char*, const char*>> uniforms) {
			elements = uniforms;
			name = material_name;
			phase = phase_name;
			shader = shader_name;
			properties = props;
		}
		bool operator==(const MaterialDescriptor& other) const = default;

		stn::array<stn::pair<const char*, const char*>> elements;
		std::string name;
		std::string phase;
		std::string shader;
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
	
	using material_handle = assets::AssetHandle<Material>;
	class MaterialManager {
		using load_descriptor = MaterialDescriptor;
	public:
		MaterialManager(uniforms::UniformManager* uniform_manager) :uniform_manager(uniform_manager) {
		}
		MaterialManager() {
		}
	
		stn::box<Material> load(const MaterialDescriptor& descriptor,assets::Assets& assets) {
			phase_handle phase = assets.from_name<render_phase>(descriptor.phase).expect("phase should exist");
			shader_id phase = assets.from_name<shader>(descriptor.shader).expect("shader should exist");


		}
		template <typename... Args>
		void construct_material(const std::string& name, phase_handle handle, assets::AssetHandle<shader> shade, const RenderProperties& props = {}, std::initializer_list<stn::pair<const char*,const char*>> uniforms) {

			if (Has(name)) {
				throw std::runtime_error("RenderType Already Created: " + name);
			}

			Material new_mat = CtxName::(name, handle, shade, props);
			(evaluate_uniform_paramater(new_mat, std::forward<Args>(args)), ...);
			new_mat.id = alloc.next();
			materials.push(new_mat);

		}


		uniforms::UniformManager* uniform_manager;
	};
}