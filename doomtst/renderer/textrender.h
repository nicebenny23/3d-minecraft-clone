#include "../math/vector2.h"

#include "../util/dynamicarray.h"
#include <string>
#include "../math/geometry.h"
#include "ui.h"
//just for the uv
#include "uibox.h"
#pragma once

namespace ui {
	
	inline void write_letter(renderer::MeshData& mesh_data, geo::Box2d location, int letter) {

		const size_t baselocation = mesh_data.length();
		for (int j = 0; j < 4; j++) {
			v2::Vec2 pointtoappend = location.scale * math::symetrical_square_mesh[j] + location.center;
			mesh_data.add_point(pointtoappend, math::square_mesh[j], letter);
		}
		mesh_data.add_indices_offset_from(math::square_mesh_triangle_indices, baselocation);

	}

	struct text_component :ecs::component {

		void set_handle() {
			if (!handle) {
				handle = CtxName::ctx.Ren->gen_renderable();
				handle.set_material("Text");
				handle.set_layout(vertice::vertex().push<float, 2>().push<float, 3>());

			}
		}
		text_component() :word("") {
		}
		void write() {
			set_handle();

			handle.enable();
			double char_offset = 1.5f;
			geo::Box2d bounds = owner().get_component<ui::ui_bounds>().global();
			v2::Vec2 min = bounds.center - v2::Vec2(char_offset * word.length(), 1.f) * bounds.half_size();
			v2::Vec2 boxoffset = v2::Vec2(char_offset, 1) * bounds.half_size();
			v2::Vec2 increse = v2::Vec2(char_offset, 0) * bounds.scale;
			geo::Box2d charlocation = geo::Box2d(min + boxoffset, bounds.scale);
			renderer::MeshData mesh_data = handle.create_mesh();
			for (int i = 0; i < word.length(); i++) {
				write_letter(mesh_data, charlocation, int(word[i] - '0'));
				charlocation.center += increse;
			}
			handle.fill(std::move(mesh_data));
			handle.set_order_key(owner().get_component<ui_priority>().priority);
		}
		renderer::RenderableHandle handle;
		std::string word;
		template<typename... Args>
		void format(const std::format_string<Args...>& fmt, Args&&... args) {
			word = std::format(fmt, std::forward<Args>(args)...);
		}
		void destroy_hook() {
			if (handle) {
				handle.destroy();
			}
		}

	};

	struct UiTextMesher:ecs::System{
		void run(ecs::Ecs& world) {

			ecs::View<ui::ui_enabled, ui::ui_bounds, ui::InteractionState, text_component> bounds_view(world);
			for (auto&& [enabled, bounds, ui_interaction, ui_text] : bounds_view) {
				if (enabled.enabled()) {
					ui_text.write();
				}
				else {
					ui_text.handle.disable();
				}
			}
		}
	};

	struct ui_text_spawner :ecs::Recipe {
		ui_spawner ui_spawn;

		ui_text_spawner(geo::Box2d box, size_t priority) :ui_spawn(geo::Box2d(box.center, box.scale), priority){
			
		}
		void apply(ecs::obj& object) {
			ui_spawn.apply(object);
			object.add_component<text_component>();
		}
	};
	struct ui_text {

		ui_text(ecs::Ecs& world, geo::Box2d bounds,size_t priority) :object(ecs::spawn_emplaced<ui::ui_text_spawner>(world, bounds, priority)) {}

		void write();
		ui_text operator=(ui_text&& other) noexcept{
			object.destroy();
			object = other.object;
			other.object = ecs::obj();
		}
		ui_text(ui_text&& other) noexcept {
			object = other.object;
			other.object = ecs::obj();
		}
		~ui_text() {
			object.destroy();
		}
		ecs::obj object;
		template<typename... Args>
		void format(const std::format_string<Args...>& fmt, Args&&... args) {
			object.get_component<text_component>().format(fmt, std::forward<Args>(args)...);
		}
		v2::Vec2 center() {
			object.get_component<ui::ui_bounds>().center();
		}
		geo::Box2d bounds() {
			return object.get_component<ui::ui_bounds>().global();
		}
		void set_center(v2::Vec2 pos) {
			object.get_component<ui::ui_bounds>().local.center = pos;
		}
		void set_bounds(geo::Box2d bounds) {
			object.get_component<ui::ui_bounds>().local = bounds;
		}
		void enable() {
			object.get_component<ui::ui_enabled>().enable();
		}
		void disable() {
			object.get_component<ui::ui_enabled>().disable();
		}
	};


	struct UiTextPlugin :Core::Plugin {
		void build(Core::App& app) {
			app.insert_plugin<UiPlugin>();
			array<std::string> texlist = array<std::string>(10);
			texlist[0] = "bitmaptext\\zeroimg.png";
			texlist[1] = "bitmaptext\\oneimg.png";
			texlist[2] = "bitmaptext\\twoimg.png";
			texlist[3] = "bitmaptext\\threeimg.png";
			texlist[4] = "bitmaptext\\fourimg.png";
			texlist[5] = "bitmaptext\\fiveimg.png";
			texlist[6] = "bitmaptext\\siximg.png";
			texlist[7] = "bitmaptext\\sevenimg.png";
			texlist[8] = "bitmaptext\\eightimg.png";
			texlist[9] = "bitmaptext\\nineimg.png";
			ecs::Ecs& world = app.Ecs;

			renderer::texture_array_id textarray = world.load_asset_emplaced<renderer::TextureArrayPath>(texlist, std::string("Letters")).unwrap();
			app.emplace_system<UiTextMesher>();
			world.ensure_resource<renderer::Renderer>().set_uniform("letters", textarray);
			world.load_asset_emplaced<renderer::shader_descriptor>("TextShader", "shaders\\textvertex.vs", "shaders\\textfragment.vs").unwrap();
			world.load_asset_emplaced<renderer::MaterialDescriptor>("Text", "ui_phase", "TextShader", renderer::RenderProperties(false, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
				stn::array{
				renderer::uparam("aspect_ratio", "aspectratio"),
				renderer::uparam("letters", "tex")
			});

		}

	};
}


