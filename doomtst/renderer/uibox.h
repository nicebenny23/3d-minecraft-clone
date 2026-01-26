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
	
	struct ui_image_component:ecs::component
	{
		ui_image_component() = default;
		renderer::RenderableHandle tex_handle;
		void start() {
		}
		
		void destroy_hook() {
			tex_handle.destroy();

		}
		void set_image(const renderer::TexturePath& path) {
			world().write_command(set_image_cmd(owner().inner(), path));
		}
	};
	struct ui_image_spawner:ecs::Recipe {
		ui_spawner ui_spawn;
		stn::Option<renderer::TexturePath> path;
		ui_image_spawner(renderer::TexturePath spawn_path,geo::Box2d box, size_t priority) :path(spawn_path),ui_spawn(geo::Box2d(box.center, box.scale),priority){
			
		}
		ui_image_spawner(geo::Box2d box, size_t priority) :path(stn::None),ui_spawn(geo::Box2d(box.center, box.scale), priority){

		}
		void apply(ecs::obj& object) {
			ui_spawn.apply(object);
			object.add_component<ui_image_component>();
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
			object.get_component<ui_enabled>().set_enabled(should_enable);	
		}
		bool enabled() {
		return 	object.get_component<ui_enabled>().enabled();
		}
		void enable() {
			object.ensure_component<ui_enabled>().enable();
		}
		void disable() {
			object.ensure_component<ui_enabled>().disable();
		}
		Box2d bounds() const {
			return object.get_component<ui::ui_bounds>().global();
		}
		void set_bounds(Box2d bounds) {
			object.get_component<ui::ui_bounds>().local = bounds;
		}
		void set_center(v2::Vec2 center) {
			object.get_component<ui::ui_bounds>().local.center = center;
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
				
					img.tex_handle = CtxName::ctx.Ren->gen_renderable();
					img.tex_handle.set_material("Ui");
					img.tex_handle.set_layout(vertice::vertex().push<float, 2>());
					renderer::MeshData mesh = img.tex_handle.create_mesh();
					mesh.add_indices(math::square_mesh_triangle_indices);
					array<float> databuf = array<float>();
					for (int j = 0; j < 4; j++) {
						mesh.add_point(math::square_mesh[j]);
					}
					img.tex_handle.fill(std::move(mesh));
				}
				img.tex_handle.set_uniform(renderer::uniform(CtxName::ctx.Ecs->load_asset(cmd.path).unwrap(), "tex"));
			}

			ecs::View<ui::ui_enabled, ui::ui_bounds, ui::ui_priority, ui_image_component> bounds_view(world);
			for (auto&& [enabled, bounds,ui_priority, ui_image] : bounds_view) {

				if (ui_image.tex_handle) {
					if (enabled.enabled()) {
						ui_image.tex_handle.enable();
						v2::Vec2 scale = bounds.global().center;
						ui_image.tex_handle.set_order_key(ui_priority.priority);
						ui_image.tex_handle.set_uniform(renderer::uniform(float(bounds.global().scale.x), "scale"));
						ui_image.tex_handle.set_uniform(renderer::uniform(bounds.global().center, "center"));

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

