#pragma once
#include "uibox.h"
#include "../math/geometry.h"
#include "../util/userinput.h"
#include "../game/Core.h"#include "../game/GameContext.h"

constexpr int listsize= 400;
const float cubeuv[] = {
	0, 1,
	1, 1,

0, 0,
1, 0

};
const v2::Vec2 offset[] = {
	v2::Vec2(-1,-1),
	v2::Vec2(1, -1),

v2::Vec2(-1, 1),
v2::Vec2(1, 1)

};
namespace uiboxname {
	void uibox::update()
	{
		state.rightclicked = false;
		state.hovered = false;
		state.leftclicked = false;
		if (mouseonblock())
		{
			state.hovered = true;
			state.leftclicked = CtxName::ctx.Inp->mouseleft().pressed;
			state.rightclicked = CtxName::ctx.Inp->mouseright().pressed;

		}
	}
	bool uibox::mouseonblock()

	{

		v2::Vec2 normedpos = CtxName::ctx.Window->FitToAspectRatio(CtxName::ctx.Inp->mousepos);
		normedpos -= box.center;
		if (abs(normedpos.x) < box.scale.x)
		{
			if (abs(normedpos.y) < box.scale.y)
			{
				return true;
			}
		}
		return false;
	}

	uibox::uibox(uibox& toreplace)
	{
		id = toreplace.id;
		tex_handle = toreplace.tex_handle;
		box = toreplace.box;

		ui::uielemlist[id].set<uibox>(*new cptr<uibox>(this));
		priority = toreplace.priority;
		state.enabled = toreplace.state.enabled;

		toreplace.destroy();
	}

	void uibox::render()
	{

		if (state.enabled)
		{
			
			tex_handle.set_uniform(uniforms::uniform(float(box.scale.x), "scale"));
			tex_handle.set_uniform(uniforms::uniform(box.center, "center"));
			tex_handle.render();
		}

	}



	void uibox::customdestroy()
	{
		tex_handle.destroy();
	}

	void uibox::LoadTex(const char* texloc, const char* texture)
	{
		if (!tex_handle)
		{
			tex_handle = CtxName::ctx.Ren->gen_renderable();
			tex_handle.set_material("Ui");
			tex_handle.set_layout(vertice::vertex().push<float, 2>());
			renderer::MeshData mesh = tex_handle.create_mesh();
			mesh.add_index(0);
			mesh.add_index(1);
			mesh.add_index(3);
			mesh.add_index(0);
			mesh.add_index(3);
			mesh.add_index(2);
			array<float> databuf = array<float>();
			for (int j = 0; j < 4; j++)
			{
				mesh.add_point(cubeuv[2 * j], cubeuv[2 * j + 1]);

			}

			tex_handle.fill(std::move(mesh));
			CtxName::ctx.Ren->pop();
		}
		tex_handle.set_uniform(uniforms::uniform(CtxName::ctx.Ren->Textures.LoadTexture(texloc, texture), "tex"));
		
	}


	uibox::uibox(const char* texloc, const char* TextureName, v2::Vec2 scl, v2::Vec2 position, float boxpriority)
	{
		LoadTex(texloc, TextureName);
		priority = boxpriority;
		box.scale = scl;
		box.center = position;
		state.enabled = true;
	}
}