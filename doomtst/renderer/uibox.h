#include "../util/sharedptr.h"
#include "ui.h"
#include "renderer.h"
#include "../util/dynamicarray.h"
#include "../math/vector2.h"
#include "../math/geometry.h"
#include "ui.h"

#include "../math/meshes.h"
#include "../game/ecs/unique_object.h"
#include "renderer/Uniform.h"
#pragma once
namespace ui {


	struct ImageMesher;
	struct Image :ecs::component {
		colors::Color current_color;
		stn::Option<renderer::TexturePath> current_image() const {
			return current_image_path;
		};
		renderer::RenderableHandle tex_handle;
		void start() {
		}
		Image(colors::Color col, renderer::RenderableHandle handle) :current_color(col), tex_handle(handle) {

		}
		void destroy_hook() {
			tex_handle.destroy();

		}
		void set_image(const renderer::TexturePath& path) {
			set_image_path = path;
		}

	private:
		stn::Option< renderer::TexturePath> set_image_path;

		stn::Option< renderer::TexturePath> current_image_path;
		friend struct ImageMesher;
	};
	struct ImageSpawner {
		UiSpawner ui_spawn;
		colors::Color color;
		stn::Option<renderer::TexturePath> path;
		ImageSpawner(const renderer::TexturePath& spawn_path, geo::Box2d box, size_t priority, colors::Color spawn_color = colors::White)
			:path(spawn_path), ui_spawn(box, priority), color(spawn_color) {

		}
		ImageSpawner(geo::Box2d box, size_t priority, colors::Color spawn_color = colors::White)
			:path(stn::None), ui_spawn(box, priority), color(spawn_color) {

		}
		void apply(ecs::obj& object) const {
			ui_spawn.apply(object);
			object.add_component<Image>(color, object.world().get_resource<Renderer>().gen_renderable("Ui"));
			if (path) {
				object.get_component<Image>().set_image(path.unwrap());

			}
		}
	};
	struct ImageMesher :ecs::System {
		ImageMesher(MeshId item_mesh) :id(item_mesh) {

		}
		MeshId id;
		void run(ecs::Ecs& world) {
			ecs::View< ui::ComputedStyle, Image, ui::UiBounds> bounds_view(world);
			for (auto&& [style, img, bounds] : bounds_view) {
				if (style.enabled) {
					img.tex_handle.set_mesh(id);
					if (img.set_image_path != img.current_image_path) {
						if (img.set_image_path) {
							img.tex_handle.set_uniform(renderer::uniform(world.load_asset(img.set_image_path.unwrap()).unwrap(), "tex"));
						}
						img.current_image_path = img.set_image_path;
					}
					img.tex_handle.set_order_key(style.priority);
					img.tex_handle.set_uniform(renderer::uniform(style.final_size.scale, "scale"));
					img.tex_handle.set_uniform(renderer::uniform(style.final_size.center, "center"));
					img.tex_handle.set_color(img.current_color);
				}
				img.tex_handle.enable_if(style.enabled);
			}
		}
	};
	struct ImagePlugin {
		void operator()(core::App& app) {
			renderer::shader_id ui_shader = app.Ecs.load_asset_emplaced<renderer::shader_descriptor>("UiShader", "shaders\\uivertex.vs", "shaders\\uifragment.vs").unwrap();

			app.Ecs.load_asset_emplaced<MaterialDescriptor>("Ui", "ui_phase", "UiShader", RenderProperties(false, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
				stn::array{ renderer::UniformRefrence("aspect_ratio", "aspectratio") }
			);
			renderer::MeshBuilder mesh = app.Ecs.get_resource<Renderer>().make_mesh_with_builder(renderer::vertex().push<float, 2>());
			mesh.add_indices(math::square_mesh_triangle_indices);
			array<float> databuf = array<float>();
			for (int j = 0; j < 4; j++) {
				mesh.add_point(math::symetrical_square_mesh[j]);
			}
			renderer::fill(std::move(mesh), app.Ecs);
			app.insert_plugin(UiPlugin());
			app.emplace_system<ImageMesher>(mesh.id());
		}

	};
}

