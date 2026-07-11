#pragma once
#include "renderer/shader.h"
#include "../math/vector3.h"
#include <glm/gtc/matrix_transform.hpp>
#include "renderer/texture.h"
#include "renderer/renderable.h"
#include "renderer/RenderProperties.h"
#include "renderer/texture_loader.h"
#include "renderer/RenderContext.h"
#include "renderer/vertex.h"
#include "../game/ecs/resources.h"
#include "../game/ecs/query.h"
#include "renderer/pass.h"
#include "../game/Core.h"
#include "../game/ecs/filtered_object.h"
#include "../player/cameracomp.h"
#include "renderer/meshStorage.h"
#include "Window.h"
#include <string>
using namespace renderer;
//Fix

namespace renderer {

	struct Renderer;
	
	struct remove_render_object {
		ecs::obj object;
	};
	struct RenderableHandle {
		RenderableHandle(renderable id)
			: id(id) {
		}
		void set_color(colors::Color color) {
			id.object().set_emplace_component<color_component>(color);
		}
		
		void set_uniform(const renderer::uniform& value) {
				id.get<MaterialComponent>().set(value);
		}		

		void enable_if(bool should_enable) {
			id.get<is_enabled>().enabled=should_enable;
		}
		void enable() {
			id.get<is_enabled>().enable();
		}
		void disable() {
				id.get<is_enabled>().disable();
		}
		void set_order_key(float key) {
			id.object().set_emplace_component<order_key>(key);
		}
		void destroy() {
			world().write_command(remove_render_object(id.object()));
		}
		bool operator==(const RenderableHandle& other) const = default;
		Renderer& renderer();
		void set_mesh(MeshId msh_id) {
			id.get<mesh_component>().msh = msh_id;
		}

		stn::Option<MeshId> mesh() const {
			return id.get<mesh_component>().msh;
		} 
		bool has_mesh() const {
			return mesh().is_some();
		}
		ecs::Ecs& world() {
			return id.world();
		}
		void give_owned_mesh();
		MeshBuilder insert_builder_for(const vertex& vertex, indice_mode auto_ind = indice_mode::manual_generate) {
			give_owned_mesh();
			return MeshBuilder(id.get<mesh_component>().msh.unwrap(), vertex, auto_ind);
		}
	
	private:
		renderable id;

	};
	inline void fill(MeshBuilder&& new_mesh,ecs::Ecs& world) {
		world.write_command(std::move(new_mesh).built_mesh());
	}


	struct Renderer : ecs::resource {

		void bind_material(MaterialHandle material) {
			gl_util::poll_errors();

			if (current_material != material) {
				Material& mat = *material;
				context.bind(*mat.shader);			

				for (auto& elem : mat.handles) {
					context.apply_uniform(uniform_manager.get(elem), std::string_view(elem.shader_alias));
				}
				context.bind_properties(mat.prop);
				current_material = material;			

			}
		}
		renderer::Context context;
		//stored the currently bound uniforms
		renderer::UniformRegistry uniform_manager;
		template<typename val_type> requires stn::OneOf<val_type, GLint,float,GLuint,v2::Vec2,v3::Vec3,glm::vec4,glm::mat3,glm::mat4,assets::AssetHandle<Texture2D>,assets::AssetHandle<TextureArray>>
		void set_uniform(const char* name, const val_type& val) {
			renderer::uniform_value value(val);
			uniform_manager.set(std::string(name), value);
		}


		void clear() {
			context.clear();
		}

		RenderableHandle gen_renderable(std::string material) {
			
			MaterialHandle handle = world.from_name<Material>(material).expect("material should exist");
			return RenderableHandle((ecs::spawn_emplaced<renderable_recipe>(world, handle)));
		}

		void render(renderer::renderable& ren) {
			bind_material(ren.get<MaterialComponent>().mat_id);
			for (auto& uniform : ren.get<MaterialComponent>().view()) {
				context.apply_uniform(uniform.value, uniform.name);
			}
			gl_util::poll_errors();

			mesh_component& mesh_comp=ren.get<mesh_component>();
			if (!mesh_comp.msh) {
				return;
			}
			if (mesh_comp.msh.unwrap()->vao.id==10) {
				shader_id id = ren.get<MaterialComponent>().mat_id->shader;
				int l = 3;
			}
			context.draw(*mesh_comp.msh.unwrap());

		}


		void fill_mesh(CpuMesh& data) {
			context.load(data);
		}

		MeshRegistry meshes;

		Renderer(ecs::Ecs& spawn_world) :world(spawn_world), uniform_manager(), meshes(context) {
			

		}
		MeshBuilder make_mesh_with_builder(renderer::vertex& vertex,renderer::indice_mode auto_ind=renderer::indice_mode::manual_generate) {
			return MeshBuilder(meshes.create_mesh_data(), vertex, auto_ind);
		}
		MeshId make_mesh() {
			return meshes.create_mesh_data();
		}
	
	private:
		stn::Option<MaterialHandle> current_material;
		ecs::Ecs& world;


	};
	inline void RenderableHandle::give_owned_mesh() {
		id.get_component<mesh_component>().msh = renderer().make_mesh();
	}
	inline Renderer& RenderableHandle::renderer() {
		return id.world().get_resource<Renderer>();
	}
	struct render_pass {
		phase_handle pass;
		//make constrained
		stn::array<renderable> phase_elements;
		void sort() {
			if (pass->should_sort) {
				phase_elements | stn::sort([](const renderable& p1) {return p1.get_unchecked<order_key>().order; });
			}
		}
		std::string_view name() const {
			return pass->name;
		}

		render_pass(phase_handle& pass) :pass(pass), phase_elements() {

		}
	};
	struct RenderAll :ecs::System {
		void run(ecs::Ecs& world) {
			ecs::Constrained<CameraComponent,core::LocalTransform> camera_object = world.get_resource<renderer::CameraResource>().camera;
			CameraComponent& cam = camera_object.get<CameraComponent>();
			Renderer& ren = world.get_resource<Renderer>();
			renderer::Window& window = world.get_resource<renderer::Window>();
			ren.set_uniform("aspect_ratio", float(window.aspect_ratio()));
			ren.set_uniform("fov", float(cam.fov.radians()));
			ren.set_uniform("view_matrix", LookAt(camera_object.get<core::LocalTransform>().transform));
			ren.set_uniform("camera_pos", v3::Vec3(camera_object.get<core::LocalTransform>().transform.position.glm()));
		
			ren.set_uniform(
				"proj_matrix", glm::perspective(float(cam.fov.radians()), float(window.aspect_ratio()), float(cam.viewport.min()), float(cam.viewport.max())
				));
			ren.set_uniform("near_plane", float(cam.viewport.min()));
			ren.set_uniform("far_plane", float(cam.viewport.max()));
			for (CpuMesh& mesh : world.read_commands<CpuMesh>()) {

				if (mesh.mesh->vao.id == 10) {
					int l = 23;

				}
				ren.fill_mesh(mesh);
			}
			std::unordered_map<phase_handle, render_pass> pass_map;
			size_t cnt = 0;

			ecs::View<renderable> renderable_iter(world);
			for (auto&& [object] : renderable_iter) {
				if (object.get<renderer::is_enabled>().enabled) {
					phase_handle pass = object.get<renderer::MaterialComponent>().mat_id->pass;
					pass_map.try_emplace(pass, render_pass(pass)).first->second.phase_elements.push(object);
					cnt++;
				}

			}
			stn::array<render_pass> passes;
			for (auto& [key, value] : pass_map) {
				passes.push(std::move(value));
			}
			pass_map.clear();
			for (render_pass& pass : passes) {
				pass.sort();
			}
			passes | stn::sort([](const render_pass& a) {return a.pass->priority; });
			for (render_pass& pass : passes) {
				for (renderable& id : pass.phase_elements) {
					ren.render(id);
				}
			}
			ren.meshes.remove_empty();
			for (remove_render_object to_remove : world.read_commands<remove_render_object>()) {
				to_remove.object.destroy();
			}
		}
	};
	struct RendererPlugin {
		void operator()(core::App& game) {
			game.insert_plugin(renderer::window_plugin);
			ecs::Ecs& world = game.Ecs;
			Renderer& renderer = world.insert_resource<Renderer>();
			world.emplace_asset_loader<renderer::TextureLoader>(renderer.context);
			world.emplace_asset_loader<renderer::TextureArrayLoader>(renderer.context);

			world.emplace_asset_loader<ShaderLoader>(renderer.context);
			world.emplace_asset_loader<assets::SelfDescriptorLoader<renderer::RenderPhase>>();
			world.load_asset(RenderPhase(2, true, "ui_phase"));
			world.load_asset(RenderPhase(0, false, "solid_phase"));
			world.load_asset(RenderPhase(1, true, "transparent_phase"));

			world.emplace_asset_loader<MaterialManager>();
			world.emplace_system<ColorSetter>();
			world.emplace_system<renderer::RenderAll>();

		}

	};

}