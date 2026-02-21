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

		const std::uint32_t baselocation = static_cast<size_t>(mesh_data.length());
		for (int j = 0; j < 4; j++) {
			v2::Vec2 pointtoappend = location.scale * math::symetrical_square_mesh[j] + location.center;
			mesh_data.add_point(pointtoappend, math::square_mesh[j], letter);
		}
		mesh_data.add_indices_offset_from(math::square_mesh_triangle_indices, baselocation);

	}

	struct text_component :ecs::component {

		void set_handle() {
			if (!handle) {
				handle = world().get_resource<renderer::Renderer>().gen_renderable();
				handle.set_material("Text");
				handle.set_layout(vertice::vertex().push<float, 2>().push<float, 3>());

			}
		}
		text_component(colors::Color color) :word(""),text_color(color) {
		}
		colors::Color text_color;
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

			ecs::View<ecs::With<ui::ComputedStyle>, ecs::With<text_component>> bounds_view(world);
			for (auto [style, ui_text] : bounds_view) {
				if (style.enabled) {
					ui_text.set_handle();
					ui_text.handle.enable();
					double char_offset = 1.5f;
					geo::Box2d bounds = style.final_size;
					v2::Vec2 min = bounds.center - v2::Vec2(char_offset * ui_text.word.length(), 1.f) * bounds.half_size();
					v2::Vec2 boxoffset = v2::Vec2(char_offset, 1) * bounds.half_size();
					v2::Vec2 increse = v2::Vec2(char_offset, 0) * bounds.scale;
					geo::Box2d charlocation = geo::Box2d(min + boxoffset, bounds.scale);
					renderer::MeshData mesh_data = ui_text.handle.create_mesh();
					for (int i = 0; i < ui_text.word.length(); i++) {
						write_letter(mesh_data, charlocation, int(ui_text.word[i] - '0'));
						charlocation.center += increse;
					}
					ui_text.handle.set_color(ui_text.text_color);
					ui_text.handle.fill(std::move(mesh_data));
					ui_text.handle.set_order_key(ui_text.owner().get_component<ComputedStyle>().priority);
				}
				else {
					if (ui_text.handle) {
						ui_text.handle.disable();
					}
				}
			}
		}
	};

	struct ui_text_spawner {
		UiSpawner ui_spawn;
		colors::Color color;
		ui_text_spawner(geo::Box2d box, size_t priority,colors::Color initial_color) :ui_spawn(geo::Box2d(box.center, box.scale), priority),color(initial_color){
			
		}
		void apply(ecs::obj& object) const{
			ui_spawn.apply(object);
			object.add_component<text_component>(color);
		}
	};

	struct UiTextPlugin :Core::Plugin {
		void build(Core::App& app) {
			app.insert_plugin<UiPlugin>();
			array<std::string> texlist = array<std::string>();
			for (size_t i = 0; i < 10; i++) {
				texlist.push(std::format("bitmaptext\\char_{}.png", i));
			}
			for (char i = 'A'; i <= 'Z';i++) {
				texlist.push(std::format("bitmaptext\\char_{}.png", i));
			}

			ecs::Ecs& world = app.Ecs;

			renderer::texture_array_id textarray = world.load_asset_emplaced<renderer::TextureArrayPath>(texlist, std::string("Letters")).unwrap();
			app.emplace_system<UiTextMesher>();
			world.get_resource<renderer::Renderer>().set_uniform("letters", textarray);
			world.load_asset_emplaced<renderer::shader_descriptor>("TextShader", "shaders\\textvertex.vs", "shaders\\textfragment.vs").unwrap();
			world.load_asset_emplaced<renderer::MaterialDescriptor>("Text", "ui_phase", "TextShader", renderer::RenderProperties(false, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
				stn::array{
				renderer::UniformRefrence("aspect_ratio", "aspectratio"),
				renderer::UniformRefrence("letters", "tex")
			});

		}

	};
}


