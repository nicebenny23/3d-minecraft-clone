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

#include "RenderContext.h"
namespace renderer {
	struct  Renderer;


	struct material_tag {
	};
	using material_id = stn::default_id< material_tag>;
	struct Material {

		std::string Name;
		RenderProperties prop;
		array<uniforms::uniform_ref> handles;
		shader_id shader;
		material_id id;
		// Default constructor
		Material(const std::string& name, shader_id shader_handle, const RenderProperties& props = {}) : Name(name), shader(shader_handle), prop(props) {
		}
	};

	class MaterialManager {

	public:
		MaterialManager(renderer::Shaders* shader_manager, uniforms::UniformManager* uniform_manager) :shader_man(shader_manager), uniform_manager(uniform_manager) {
		}
		MaterialManager() {
		}
		// Check if a RenderType with the given name exists
		bool Has(const std::string& name) const {
			return materials.any([&](const Material& mat) {return mat.Name == name; });
		}

		template <typename... Args>
		void construct_material(const std::string& name, const std::string& shade, const RenderProperties& props = {}, Args&&... args) {

			if (Has(name)) {
				throw std::runtime_error("RenderType Already Created: " + name);
			}

			auto val = Material(name, shader_man->handle_of(shade), props);
			(evaluate_uniform_paramater(val, std::forward<Args>(args)), ...);
			val.id = alloc.next();
			materials.push(val);

		}

		void evaluate_uniform_paramater(Material& mat, stn::pair<const char*, const char*> uniform_ref) {
			mat.handles.push(uniforms::uniform_ref(uniform_manager->get_handle(uniform_ref.first), uniform_ref.second));
		}



		const material_id get_id(const std::string& name) const {
			Option<const Material&> mat = materials.such_that([&](const Material& mat) {return mat.Name == name; });
			return mat.expect("material does not exist").id;
		}
		const Material& operator[](const material_id& id) const {

			return materials[id.get()];
		}
		// Clear all stored RenderTypes
		void Clear() {

		}

		// Destructor
		~MaterialManager() = default;
	private:
		stn::counter<material_id> alloc;
		stn::array<Material> materials;
		renderer::Shaders* shader_man;
		uniforms::UniformManager* uniform_manager;
	};
}