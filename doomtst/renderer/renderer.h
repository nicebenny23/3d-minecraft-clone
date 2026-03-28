#pragma once
#include "renderer/Vao.h"
#include "renderer/vertexobject.h"
#include "renderer/shader.h"
#include <glm/mat4x4.hpp>
#include "../math/vector3.h"
#include <glm/gtc/matrix_transform.hpp>
#include "renderer/texture.h"
#include "renderer/renderable.h"
#include "renderer/Mesh.h"
#include "renderer/RenderProperties.h"
#include "../util/stack.h"
#include "renderer/RenderContext.h"
#include "renderer/vertex.h"
#include "../game/ecs/resources.h"
#include "../util/Id.h"
#include "../game/ecs/query.h"
#include "renderer/pass.h"
#include "../game/Core.h"
#include "../util/Name.h"
#include "../game/ecs/filtered_object.h"
#include "../game/Settings.h"
#include "../player/cameracomp.h"
#include "renderer/meshStorage.h"
using namespace renderer;
//Fix

namespace renderer {

	struct Renderer;
	struct RenderableHandle {
		RenderableHandle() :id() {
		}
		RenderableHandle(renderable id)
			: id(id) {
		}
		void set_color(colors::Color color) {
			if (id) {
				id.unwrap().object().set_emplace_component<color_component>(color);
			}
		}
		void fill(MeshData&& new_mesh) {
			if (id) {
				id.unwrap().world().write_command(std::move(new_mesh));
			}
			else {
				stn::throw_logic_error("cannot fill uninitialized mesh");
			}
		}

		void set_uniform(const renderer::uniform& value) {
			if (id) {
				id.unwrap().get<renderable_overides>().set(value);
			}
		}

		void enable() {
			if (id) {
				id.unwrap().get<is_enabled>().enable();
			}
		}
		void disable() {
			if (id) {
				id.unwrap().get<is_enabled>().disable();
			}
		}
		void set_order_key(float key) {
			if (id) {
				id.unwrap().object().set_emplace_component<order_key>(key);
			}
		}
		mesh_id mesh();
		void destroy();
		bool operator==(const RenderableHandle& other) const = default;
		Renderer& renderer();
		MeshData create_mesh(vertice::vertex& mesh, indice_mode auto_ind = indice_mode::manual_generate);
		explicit operator bool() const noexcept {
			return static_cast<bool>(id);
		}
	private:
		Option<renderable> id;

	};

	struct remove_render_object {
		ecs::obj object;
	};

	struct Renderer : ecs::resource {

		void bind_material(material_handle material) {
			GlUtil::poll_errors();

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

		void Bind_texture(texture_2d_id Handle) {
			context.bind(*Handle);
		}
		void Bind_texture(texture_array_id Handle) {
			context.bind(*Handle);
		}
		renderer::Context context;
		//stored the currently bound uniforms
		renderer::UniformRegistry uniform_manager;
		template<typename val_type>
		void set_uniform(const char* name, const val_type& val) {
			uniform_manager.set(name, renderer::uniform_value(std::in_place_type<val_type>, val));
		}


		void clear() {
			context.clear();
		}

		RenderableHandle gen_renderable(std::string material) {
			
			material_handle handle = world.from_name<Material>(material).expect("material should exist");
			return RenderableHandle((ecs::spawn_emplaced<renderable_recipe>(world, handle)));
		}
		void remove(renderable ren) {
			ren.world().write_command(remove_render_object(ren.object()));
		}

		void render(renderer::renderable& ren) {

			bind_material(ren.get<material_component>().mat_id);
			for (auto& uniform : ren.get<renderable_overides>().view()) {
				context.apply_uniform(uniform.value, uniform.name);
			}
			GlUtil::poll_errors();

			Mesh& mesh = *ren.get<mesh_component>().msh.expect("all must have a mesh");
			if (!mesh.filled()) {
				return;
			}
			context.bind(mesh);
			if (world.ensure_resource<settings::GlobalSettings>().viewmode) {
				glDrawElements(GL_LINES, mesh.length, GL_UNSIGNED_INT, 0);
			}
			else {
				glDrawElements(GL_TRIANGLES, mesh.length, GL_UNSIGNED_INT, 0);
			};		

		}


		void fill_mesh(MeshData& data) {
			meshes.load_in(data);
		}

		MeshRegistry meshes;

		Renderer(ecs::Ecs& spawn_world) :world(spawn_world), uniform_manager() {
			meshes = MeshRegistry(&context);

		}
		mesh_id insert_mesh(renderer::renderable id) {
			stn::Option<mesh_id>& msh_id= id.get_component<mesh_component>().msh;
			msh_id.fallback_on([&]() {
				return meshes.create_mesh_data(); });
			return msh_id.unwrap();
		}
	private:
		stn::Option<material_handle> current_material;
		ecs::Ecs& world;


	};
	inline Renderer& RenderableHandle::renderer() {
		return id.unwrap().world().get_resource<Renderer>();
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
	struct render_all :ecs::System {
		void run(ecs::Ecs& world) {
			ecs::Constrained<CameraComponent> camera_object = world.get_resource<renderer::camera_resource>().camera;
			CameraComponent& cam = camera_object.get_component<CameraComponent>();
			Renderer& ren = world.get_resource<Renderer>();
			window::Window& window = world.get_resource<window::Window>();
			ren.set_uniform("aspect_ratio", window.AspectRatio());
			ren.set_uniform("fov", float(cam.fov));
			ren.set_uniform("view_matrix", LookAt(camera_object.get_component<ecs::world_transform>().transform));
			ren.set_uniform("camera_pos", v3::Vec3(camera_object.get_component<ecs::world_transform>().transform.position.glm()));

			ren.set_uniform(
				"proj_matrix", glm::perspective(float(glm::radians(cam.fov)), window.AspectRatio(), float(cam.viewport.min()), float(cam.viewport.max())
				));
			ren.set_uniform("near_plane", float(cam.viewport.min()));
			ren.set_uniform("far_plane", float(cam.viewport.max()));
			for (MeshData& mesh : world.read_commands<MeshData>()) {

				ren.fill_mesh(mesh);
			}
			std::unordered_map<phase_handle, render_pass> pass_map;
			size_t cnt = 0;

			ecs::View<ecs::With<material_component>, ecs::With<order_key>, ecs::With<is_enabled>, ecs::Owner> renderable_iter(world);
			for (auto [mat, order, should_render, object] : renderable_iter) {
				if (should_render.enabled) {
					phase_handle pass = mat.mat_id->pass;
					pass_map.try_emplace(pass, render_pass(pass));
					pass_map.at(pass).phase_elements.push(object);
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
	struct RendererPlugin :Core::Plugin {
		void build(Core::App& game) {

			ecs::Ecs& world = game.Ecs;

			world.emplace_asset_loader<renderer::TextureLoader>();
			world.emplace_asset_loader<renderer::TextureArrayLoader>();
			world.emplace_asset_loader<shader_loader>();
			world.emplace_asset_loader<assets::SelfDescriptorLoader<renderer::render_phase>>();
			Renderer& renderer = world.insert_resource<Renderer>();
			world.emplace_asset_loader<MaterialManager>();
			renderer::shader_id ui_shader = world.load_asset_emplaced<renderer::shader_descriptor>("UiShader", "shaders\\uivertex.vs", "shaders\\uifragment.vs").unwrap();
			world.load_asset(render_phase(12, true, "ui_phase"));
			world.load_asset(render_phase(0, false, "solid_phase"));
			world.load_asset(render_phase(1, true, "transparent_phase"));
			world.emplace_system<ColorSetter>();
			world.load_asset_emplaced<MaterialDescriptor>("Ui", "ui_phase", "UiShader", RenderProperties(false, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
				stn::array{ renderer::UniformRefrence("aspect_ratio", "aspectratio") }
			);
			world.emplace_system<renderer::render_all>();
			renderer::shader_id model_shader = world.load_asset_emplaced<renderer::shader_descriptor>("ModelShader", "shaders\\modelvertex.vs", "shaders\\modelfragment.vs").unwrap();
			world.load_asset_emplaced<MaterialDescriptor>("Model", "solid_phase", "ModelShader", RenderProperties(true, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
				stn::array<renderer::UniformRefrence>{
				renderer::UniformRefrence("aspect_ratio", "aspectratio"),
					renderer::UniformRefrence("proj_matrix", "projection"),
					renderer::UniformRefrence("view_matrix", "view")
			});

		}

	};

}