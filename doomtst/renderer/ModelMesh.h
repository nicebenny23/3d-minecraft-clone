#pragma once
#include "../util/dynamicarray.h"
#include "../util/fileloader.h"
#include "../math/vector2.h"
#include "../math/vector3.h"

 
#include "../math/transform.h"
#include "renderer.h"
#include "../game/Core.h"
#include "renderer/vertex.h"
//fix
using namespace stn;
using namespace v3;
using namespace renderer;
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#define _CRT_SECURE_NO_DEPRECATE
namespace renderer {
	struct MeshPath {
		MeshPath(std::string_view mesh) :mesh(mesh) {

		}
		std::string mesh;
		bool operator==(const MeshPath& other) const = default;

	};
	struct ModelPath {
		MeshPath mesh;
		TexturePath texture;
		bool operator==(const ModelPath& other) const = default;

	};

	struct DecalTag :ecs::component {

	};
	
	struct Model:ecs::component {
		bool enabled=true;
		Model(renderer::RenderableHandle handle,ModelPath pth) :handle(handle),path(pth){

		}
		renderer::RenderableHandle handle;
		colors::Color color;
		stn::Option<Texture2dId> texture;
		ModelPath path;
		stn::Option<ModelPath> loaded_path;
		void destroy_hook() override {
			handle.destroy();
		}
	};

	struct ModelRecipe {
		ModelPath path;
		void apply(ecs::obj& object) const {
			object.add_component<core::LocalTransform>(v3::Point3(0,0,0));
			Model& m=object.add_component<Model>(object.world().get_resource<renderer::Renderer>().gen_renderable("Model"),path);
			m.texture = object.world().load_asset(path.texture).unwrap();
			
		}
	};
	struct DecalRecipe {
		TexturePath path;
		void apply(ecs::obj& object) const {
			object.add_component<DecalTag>();
			object.apply_recipe(ModelRecipe{ .path = ModelPath{.mesh = MeshPath("meshes\\decal.obj"),.texture = path} });
		}
	};
	struct ModelMesher:ecs::System{
		void run(ecs::Ecs& world) {
			ecs::View<Model, core::LocalTransform, ecs::Mabye<DecalTag>> world_view(world);
			for (auto&& [model, transform, decal_tag] : world_view) {
				if (model.loaded_path != model.path) {
					file_handle meshfile = stn::file_handle(model.path.mesh.mesh, stn::FileMode::Read);
					MeshBuilder mesh = model.handle.insert_builder_for(renderer::vertex().push<float, 3>().push<float, 2>(), renderer::indice_mode::generate_indices);
					stn::array<v2::Vec2> tex_coords;
					stn::array<v3::Point3> points;
					while (true) {
						char* header = new char[128];
						int test = fscanf(meshfile.fp, "%s", header);
						if (test == EOF) {
							break;
						}
						if (strcmp(header, "v") == 0) {
							float c1 = 0, c2 = 0, c3 = 0;
							meshfile.fscanf("%f %f %f\n", &c1, &c2, &c3);
							points.push(Point3(c1, c2, c3));
						}
						if (strcmp(header, "vt") == 0) {

							float c1 = 0, c2 = 0;
							meshfile.fscanf("%f %f\n", &c1, &c2);

							tex_coords.push(v2::Vec2(c1, c2));
						}
						if (strcmp(header, "f") == 0) {
							unsigned int vertexIndex[3] = { 0,0,0 };
							unsigned int uvIndex[3] = { 0,0,0 };
							meshfile.fscanf("%u/%u %u/%u %u/%u\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]);
							for (int i = 0; i < 3; i++) {
								mesh.add_point(points[vertexIndex[i] - 1], tex_coords[uvIndex[i] - 1]);
							}
						}
						delete[] header;
					}
					model.loaded_path = model.path;
					renderer::fill(std::move(mesh), world);
				}
				if (model.texture&&model.enabled) {
					model.handle.set_color(model.color);
					v3::Point3 cam_pnt = world.get_resource<renderer::CameraResource>().center();
					model.handle.set_order_key(v3::dist(cam_pnt, transform.transform.position));
					model.handle.set_uniform(uniform(model.texture.unwrap(), "tex"));
					model.handle.enable();

					math::Transform result_transform = transform.transform;
					if (decal_tag.is_some()) {
 						result_transform.position += result_transform.normal_dir() * .01f;
					}
					model.handle.set_uniform(uniform(result_transform.as_matrix(), "model"));
				}
				else {
					model.handle.disable();
				}
			}
		}
	};
	

	inline void model_plugin(core::App& app){
			renderer::shader_id model_shader = app.Ecs.load_asset_emplaced<renderer::shader_descriptor>("ModelShader", "shaders\\modelvertex.vs", "shaders\\modelfragment.vs").unwrap();
			app.Ecs.load_asset_emplaced<MaterialDescriptor>("Model", "solid_phase", "ModelShader", RenderProperties(true, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
				stn::array<renderer::UniformRefrence>{
					renderer::UniformRefrence("proj_matrix", "projection"),
					renderer::UniformRefrence("view_matrix", "view")
			});
			app.emplace_system<ModelMesher>();
	};
	
}
