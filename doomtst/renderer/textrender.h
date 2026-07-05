#include "../math/vector2.h"

#include "../util/dynamicarray.h"
#include <string>
#include "../math/geometry.h"
#include "ui.h"
//just for the uv
#include "uibox.h"
#pragma once

namespace ui {
	inline const char* letters= "0123456789abcdefghijklmnopqrstuvABCDEFGHIJKLMNOPQRSTUVWXYZ.,-_?!+=/;: ";
	inline size_t symbol_index_for(char index) {
		for (size_t i = 0; i < std::strlen(letters); i++) {
			if (index==letters[i]) {
				return i;
			}
		}
		stn::throw_logic_error("char {} does not have a symbol",index);
	}
	inline void write_letter(renderer::MeshBuilder& mesh_data, geo::Box2d location, size_t letter) {

		const std::uint32_t baselocation = static_cast<size_t>(mesh_data.vertex_count());
		for (int j = 0; j < 4; j++) {
			v2::Vec2 pointtoappend = location.scale * math::symetrical_square_mesh[j] + location.center;
			mesh_data.add_point(pointtoappend, math::square_mesh[j], letter);
		}
		mesh_data.add_indices_offset_from(math::square_mesh_triangle_indices, baselocation);

	}

	struct Text :ecs::component {

		Text(colors::Color color,renderer::RenderableHandle handle) :word(""),text_color(color), handle(handle){
		}
		colors::Color text_color;
		renderer::RenderableHandle handle;
		std::string word;
		v2::Vec2 spacing_gap=v2::Vec2(.2f,1.0f);
		template<typename... Args>
		void format(const std::format_string<Args...>& fmt, Args&&... args) {
			word = std::format(fmt, std::forward<Args>(args)...);
		}
		void destroy_hook() {
				handle.destroy();
		}

	};

	struct TextMesher:ecs::System{
		void run(ecs::Ecs& world) {

			ecs::View<ui::ComputedStyle,Text> bounds_view(world);
			for (auto [style, ui_text] : bounds_view) {
				if (style.enabled) {
					v2::Vec2 move_offset=ui_text.spacing_gap+v2::unitv;
					v2::UVec2 size(0, 0);
					size_t row_len=0;
					renderer::MeshBuilder mesh_data = ui_text.handle.insert_builder_for(renderer::vertex().push<float, 2>().push<float, 3>());
					if (!ui_text.word.empty()) {
						size.y += 1;
					}
					for (char symbol:ui_text.word) {
						if (symbol=='\n') {
							row_len = 0;
							size.y += 1;
						}
						else {
							v2::Vec2 point = v2::Vec2(.5 + row_len * move_offset.x, -.5- move_offset.y * (size.y-1));
							int key = symbol_index_for(symbol);
							write_letter(mesh_data, geo::Box2d(point,v2::unitv), key);
							row_len++;
							stn::set_max(size.x, row_len);
						}
					}
					double x = size.x + (size.x - 1) * ui_text.spacing_gap.x;

					double y= size.y + (size.y - 1) * ui_text.spacing_gap.y;
					v2::Vec2 center = v2::Vec2(x,-y) / 2;
					ui_text.handle.set_color(ui_text.text_color);
					renderer::fill(std::move(mesh_data), world);
					ui_text.handle.set_order_key(style.priority);
					ui_text.handle.set_uniform(renderer::uniform(style.final_size.scale, "scale"));
					ui_text.handle.set_uniform(renderer::uniform(style.final_size.center-center*style.final_size.scale/2, "center"));
				
				}
				
				ui_text.handle.enable_if(style.enabled);

			}
		}
	};

	struct TextSpawner {
		UiSpawner ui_spawn;
		colors::Color color;
		TextSpawner(geo::Box2d box, size_t priority,colors::Color initial_color=colors::White) :ui_spawn(geo::Box2d(box.center, box.scale), priority),color(initial_color){
			
		}
		void apply(ecs::obj& object) const{
			ui_spawn.apply(object);
			object.add_component<Text>(color,object.world().get_resource<renderer::Renderer>().gen_renderable("Text"));
		}
	};

	struct TextPlugin {
		void operator()(core::App& app) {
			app.insert_plugin(UiPlugin());
			array<std::string> texlist = array<std::string>();
			for (size_t i = 0; i < strlen(letters); i++) {
				texlist.push(std::format("bitmaptext\\char_{}.png", i));
			}
			ecs::Ecs& world = app.Ecs;

			renderer::TextureArrayId textarray = world.load_asset_emplaced<renderer::TextureArrayPath>(texlist, std::string("Letters")).unwrap();
			app.emplace_system<TextMesher>();
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


