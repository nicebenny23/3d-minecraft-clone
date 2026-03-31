#pragma once
#include "../util/dynamicarray.h"
#include "../util/fileloader.h"
#include "../math/vector2.h"
#include "../math/vector3.h"

 
#include "../math/transform.h"
#include "renderer.h"
#include "../game/Core.h"
//fix
using namespace stn;
using namespace v3;
using namespace renderer;
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS

#define _CRT_SECURE_NO_DEPRECATE
namespace renderer {
	struct ModelPath {
		std::string mesh;
		TexturePath texture;

	};
	struct Model:ecs::component {
		renderer::RenderableHandle handle;
		colors::Color color;
		texture_2d_id texture;
		Model(texture_2d_id tex) :texture(tex) {

		}
		void destroy_hook() override {
			handle.destroy();
		}
	};

	struct LoadMeshCommand {
		ModelPath path;
		ecs::Constrained<Model> model;
	};
	struct ModelRecipe {
		ModelPath path;
		void apply(ecs::obj& object) const {
			object.add_component<Model>(object.world().load_asset(path.texture).unwrap());
			object.world().write_command(LoadMeshCommand{ .path = path,.model = object });
		}
	};
	struct ModelMesher:ecs::System{
		void run(ecs::Ecs& world) {
			for (LoadMeshCommand& cmd : world.read_commands< LoadMeshCommand>()) {


				file_handle meshfile = stn::file_handle(cmd.path.mesh, stn::FileMode::Read);
				Model& loaded_model = cmd.model.get<Model>();
				if (!loaded_model.handle) {
					loaded_model.handle = world.get_resource<Renderer>().gen_renderable("Model");
				}
				MeshBuilder mesh = loaded_model.handle.create_mesh(vertice::vertex().push<float, 3>().push<float, 2>(),renderer::indice_mode::generate_indices);
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
							mesh.add_point(points[vertexIndex[i]-1], tex_coords[uvIndex[i]-1]);
						}
					}
					delete[] header;
				}

				loaded_model.handle.fill(std::move(mesh));
			}
			ecs::View<Model,ecs::world_transform> world_view(world);
			for (auto&& [model, transform] : world_view) {
				if (model.handle) {
					model.handle.set_color(model.color);
					v3::Point3 cam_pnt = world.get_resource<renderer::camera_resource>().camera.get<CameraComponent>().CamTransform.position;
					model.handle.set_order_key(v3::dist(cam_pnt, transform.transform.position));
					model.handle.set_uniform(uniform(model.texture, "tex"));
					model.handle.enable();
					model.handle.set_uniform(uniform(transform.transform.as_matrix(), "model"));
				}
			}
		};
	};
	

	struct ModelPlugin:Core::Plugin{
		void build(Core::App& app) {
			app.emplace_system<ModelMesher>();
		}
	};
	
}
