#include "../util/sharedptr.h"
#include "ui.h"
#include "renderer.h"
#include "../util/dynamicarray.h"
#include "../math/vector2.h"
#include "../math/geometry.h"
#include "ui.h"

#include "../math/meshes.h"
#include "../game/ecs/unique_object.h"
using namespace stn;
using namespace geo;
#pragma once
namespace ui {

	struct set_image_cmd {
		set_image_cmd(ecs::entity ui_ent, const renderer::TexturePath& texture_path) :path(texture_path), ui_entity(ui_ent) {}
		renderer::TexturePath path;
		ecs::entity ui_entity;
	};
	struct prepare_ui_image;
	struct ui_image_component:ecs::component
	{
		ui_image_component() = default;
		colors::Color current_color;
		stn::Option<renderer::TexturePath> current_image() const{
			return current_image_path;
		};
		renderer::RenderableHandle tex_handle;
		void start() {
		}
		ui_image_component(colors::Color col) :current_color(col) {

		}
		void destroy_hook() {
			tex_handle.destroy();

		}
		void set_image(const renderer::TexturePath& path) {
			world().write_command(set_image_cmd(owner().inner(), path));
		}

	private:
	stn::Option< renderer::TexturePath> current_image_path;
	friend struct prepare_ui_image;
	};
	struct ui_image_spawner{
		UiSpawner ui_spawn;
		colors::Color color;
		stn::Option<renderer::TexturePath> path;
		ui_image_spawner(const renderer::TexturePath& spawn_path,geo::Box2d box, size_t priority, colors::Color spawn_color=colors::White)
		:path(spawn_path),ui_spawn(box,priority), color(spawn_color) {
			
		}
		ui_image_spawner(geo::Box2d box, size_t priority,colors::Color spawn_color = colors::White)
		:path(stn::None),ui_spawn(box, priority),color(spawn_color){

		}
		void apply(ecs::obj& object) const{
			ui_spawn.apply(object);
			object.add_component<ui_image_component>(color);
			if (path) {
				object.get_component<ui_image_component>().set_image(path.unwrap());

			}
		}
	};
	struct ui_image{
		ui_image(ecs::Ecs& world,const char* texloc, const char* texture, geo::Box2d bounds, size_t prio) :object(ecs::spawn_emplaced<ui_image_spawner>(world,renderer::TexturePath(std::string(texloc),std::string(texture)),bounds,prio)){
		}
		ecs::object_handle object;
		void enable_if(bool should_enable) {
			object.get_component<UiEnabled>().set_enabled(should_enable);	
		}
		
		void enable() {
			object.set_emplace_component<UiEnabled>().enable();
		}
		void disable() {
			object.set_emplace_component<UiEnabled>().disable();
		}
		
		void set_bounds(Box2d bounds) {
			object.get_component<ui::UiBounds>().local = bounds;
		}
		void set_center(v2::Vec2 center) {
			object.get_component<ui::UiBounds>().local.center = center;
		}

		void set_image(const char* texloc, const char* TextureName) {
			object.get_component<ui_image_component>().set_image(renderer::TexturePath(texloc, TextureName));
		}
		void add_child(const ecs::obj &child) {
			object.get().add_child(child);
		}
		void make_child_of(ecs::obj& parent) {
			parent.add_child(object.get());
		}
		template<ecs::ComponentType T>
		T& get_component() {
			return object.get_component<T>();
		}
		

	};
	struct prepare_ui_image :ecs::System {
		void run(ecs::Ecs& world) {
			for (const set_image_cmd& cmd:world.read_commands<set_image_cmd>()){
				if (!world.contains(cmd.ui_entity)) {
					continue;
				}
				ui_image_component& img= world.get_component<ui_image_component>(cmd.ui_entity);
				if (!img.tex_handle) {
					img.tex_handle = world.get_resource<renderer::Renderer>().gen_renderable("Ui");
					img.tex_handle.set_layout(vertice::vertex().push<float, 2>());
					renderer::MeshData mesh = img.tex_handle.create_mesh();
					mesh.add_indices(math::square_mesh_triangle_indices);
					array<float> databuf = array<float>();
					for (int j = 0; j < 4; j++) {
						mesh.add_point(math::square_mesh[j]);
					}
					img.tex_handle.fill(std::move(mesh));
				}
				if (cmd.path!=img.current_image_path) {
					img.tex_handle.set_uniform(renderer::uniform(world.load_asset(cmd.path).unwrap(), "tex"));
					img.current_image_path = cmd.path;
				}
			}

			ecs::View<ecs::With<ui::ComputedStyle>, ecs::With<ui_image_component>> bounds_view(world);
			for (auto&& [style, ui_image] : bounds_view) {

				if (ui_image.tex_handle) {
					if (style.enabled) {
						ui_image.tex_handle.enable();
						ui_image.tex_handle.set_order_key(style.priority);
						ui_image.tex_handle.set_uniform(renderer::uniform(style.final_size.scale, "scale"));
						ui_image.tex_handle.set_uniform(renderer::uniform(style.final_size.center, "center"));
						ui_image.tex_handle.set_color(ui_image.current_color);
					}
					else {
						ui_image.tex_handle.disable();
					}
				}
			}
		}
	};
	struct UiImagePlugin :Core::Plugin {
		void build(Core::App& app) {
			app.insert_plugin<UiPlugin>();
			app.emplace_system<prepare_ui_image>();
		}

	};
}

