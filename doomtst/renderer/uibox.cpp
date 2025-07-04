#pragma once
#include "uibox.h"
#include "../util/geometry.h"
#include "../util/userinput.h"
#include "../game/Core.h"#include "../game/GameContext.h"

constexpr int listsize= 400;
const float cubeuv[] = {
	0, 1,
	1, 1,

0, 0,
1, 0

};
const v2::Vector2 offset[] = {
	v2::Vector2(-1,-1),
	v2::Vector2(1, -1),

v2::Vector2(-1, 1),
v2::Vector2(1, 1)

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

		v2::Vector2 normedpos = CtxName::ctx.Window->FitToAspectRatio(CtxName::ctx.Inp->mousepos);
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
		tex = toreplace.tex;
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
			renderer::changerendertype(renderer::renderui);
			array<unsigned int> indbuf = array<unsigned int>();
			indbuf[0] = 0;
			indbuf[1] = 1;
			indbuf[2] = 3;
			indbuf[3] = 0;
			indbuf[4] = 3;
			indbuf[5] = 2;

			Mesh BoxMesh;
			renderer::Ren.Gen<true>(&BoxMesh);
			BoxMesh.AddAttribute<float, 2>().AddAttribute<float, 2>();
			array<float> databuf = array<float>();
			renderer::Ren.Textures.Apply(tex);
			for (int j = 0; j < 4; j++)
			{
				v2::Vector2 pos = box.center + offset[j] * (box.scale);
				databuf.push(pos.x);
				databuf.push(pos.y);
				databuf.push(cubeuv[2 * j]);
				databuf.push(cubeuv[2 * j + 1]);
			}
			renderer::Ren.Render(&BoxMesh, databuf, indbuf);
			databuf.destroy();
			indbuf.destroy();
			renderer::Ren.Destroy(&BoxMesh);
		}

	}



	void uibox::customdestroy()
	{
	}

	void uibox::LoadTex(const char* texloc, const char* texture)
	{
		tex = renderer::Ren.Textures.Get2dTex(texloc, texture);

	}


	uibox::uibox(const char* texloc, const char* TextureName, v2::Vector2 scl, v2::Vector2 position, float boxpriority)
	{
		LoadTex(texloc, TextureName);
		priority = boxpriority;
		box.scale = scl;
		box.center = position;
		state.enabled = true;
	}
}