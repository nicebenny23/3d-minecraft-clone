
#pragma once
#include "../../util/dynamicarray.h"
#include "../../util/fileloader.h"
#include "../../math/vector2.h"
#include "../../math/vector3.h"


#include "../../math/transform.h"
#include "../renderer.h"
#include "../../game/Core.h"
#include "../../game/ecs/component.h"
#include "../../math/meshes.h"
	//fix

namespace renderer {
		struct WireFrame:ecs::component {
			WireFrame(renderer::RenderableHandle handle) :handle(handle) {

			}
			renderer::RenderableHandle handle;
			colors::Color color;
			bool enabled=true;
			void destroy_hook() override {
				handle.destroy();
			}
		};

		
		struct WireFrameRenderer:ecs::System {
			MeshId world_id;
			WireFrameRenderer(MeshId id):world_id(id) {
			
			}
			void run(ecs::Ecs& world) {
				renderer::Renderer& renderer = world.get_resource<renderer::Renderer>();
				ecs::View<WireFrame, core::LocalTransform> world_view(world);
				for (auto&& [model, transform] : world_view) {
					if ( model.enabled) {
						if (!model.handle.has_mesh()) {
							model.handle.set_mesh(world_id);
						}
						model.handle.set_color(model.color);
						v3::Point3 cam_pnt = world.get_resource<renderer::CameraResource>().center();
						model.handle.enable();
		
						math::Transform result_transform = transform.transform;
						model.handle.set_uniform(uniform(result_transform.as_matrix(), "model"));
					}
					else {
						model.handle.disable();
					}
				}
			};
		};

		inline void wireframe_plugin(core::App& app) {

			renderer::shader_id model_shader = app.Ecs.load_asset_emplaced<renderer::shader_descriptor>("WireframeShader", "shaders\\wireframe_vert.vs", "shaders\\wireframe_frag.vs").unwrap();
			app.Ecs.load_asset_emplaced<MaterialDescriptor>("Wireframe", "solid_phase", "WireframeShader", RenderProperties(true, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_LINES),
				stn::array<renderer::UniformRefrence>{
					renderer::UniformRefrence("proj_matrix", "projection"),
					renderer::UniformRefrence("view_matrix", "view")
			});
			renderer::Renderer& renderer= app.Ecs.get_resource<renderer::Renderer>();
			MeshBuilder mesh = renderer.make_mesh_with_builder(renderer::vertex().push<float, 3>(), indice_mode::manual_generate);
			mesh.add_indices(math::cube_edge_indices);
			for (v3::Coord point : math::cube_mesh) {
				mesh.add_point(point - v3::unitv / 2.0);
			}
			renderer::fill(std::move(mesh), app.Ecs);
			app.emplace_system< WireFrameRenderer>(mesh.id());
		}; 

		inline void wireframe_recipe(ecs::obj& object) {
			core::TransformRecipe(v3::Point3(0,0,0)).apply(object);
			object.add_component< WireFrame>(object.world().get_resource<Renderer>().gen_renderable("Wireframe"));
		}
}
