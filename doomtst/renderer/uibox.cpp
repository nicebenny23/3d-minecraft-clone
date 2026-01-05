#pragma once
#include "uibox.h"
#include "../math/geometry.h"
#include "../util/userinput.h"
#include "../game/Core.h"#include "../game/GameContext.h"

constexpr int listsize= 400;
const v2::Vec2 cubeuv[] = {
v2::Vec2(0, 1),
v2::Vec2(1, 1),
v2::Vec2(0, 0),
v2::Vec2(1, 0)
};

namespace uiboxname {
	

	void ui_image_component::destroy_hook()
	{
		tex_handle.destroy();
	}

	void ui_image_component::LoadTex(const char* texloc, const char* texture)
	{
		if (!tex_handle)
		{
			tex_handle = CtxName::ctx.Ren->gen_renderable();
			tex_handle.set_material("Ui");
			tex_handle.set_layout(vertice::vertex().push<float, 2>());
			renderer::MeshData mesh = tex_handle.create_mesh();
			mesh.add_indices({ 0,1,3,0,3,2 });
			array<float> databuf = array<float>();
			for (int j = 0; j < 4; j++)
			{
				mesh.add_point(cubeuv[j]);
			}
			tex_handle.fill(std::move(mesh));
		}
		tex_handle.set_uniform(uniforms::uniform(CtxName::ctx.Ecs->load_asset_emplaced<TexturePath>(texloc, texture).unwrap(), "tex"));
	//	tex_handle.set_order_key(priority);


	}


	ui_image_component::ui_image_component(const char* texloc, const char* TextureName, v2::Vec2 scl, v2::Vec2 center, float boxpriority):handle(*CtxName::ctx.Ecs, geometry::Box2d(center, scl))
	{
		LoadTex(texloc, TextureName);	
		handle.set_bounds(geometry::Box2d(center, scl));
		enable();
	}
	struct prepare_ui_image:ecs::System{
		void run(ecs::Ecs& world) {

			ecs::View<ui::ui_enabled, ui::ui_bounds, ui::InteractionState, ui_image_component> bounds_view(world);
			for (auto&& [enabled, bounds, ui_interaction, ui_image] : bounds_view) {
				if (enabled.enabled()) {

					ui_image.tex_handle.set_uniform(uniforms::uniform(float(bounds.global().scale.x), "scale"));
					ui_image.tex_handle.set_uniform(uniforms::uniform(bounds.global().center, "center"));
				}
				else {
					ui_image.tex_handle.disable();
				}
			}
		}
	};
}