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
			CtxName::ctx.Ren->SetType("Ui");
			array<unsigned int> indbuf = array<unsigned int>(6,unsigned int(0));
			indbuf[0] = 0;
			indbuf[1] = 1;
			indbuf[2] = 3;
			indbuf[3] = 0;
			indbuf[4] = 3;
			indbuf[5] = 2;

			Mesh BoxMesh;
			CtxName::ctx.Ren->Gen(&BoxMesh);
			BoxMesh.Voa.attributes.push<float, 2>().push<float, 2>();
			array<float> databuf = array<float>();
			CtxName::ctx.Ren->context.Bind(*tex);
			for (int j = 0; j < 4; j++)
			{
				v2::Vec2 pos = box.center + offset[j] * (box.scale);
				databuf.push(pos.x);
				databuf.push(pos.y);
				databuf.push(cubeuv[2 * j]);
				databuf.push(cubeuv[2 * j + 1]);
			}
			CtxName::ctx.Ren->Render(&BoxMesh, databuf, indbuf);
			databuf.destroy();
			indbuf.destroy();
			CtxName::ctx.Ren->Destroy(&BoxMesh);
		}

	}



	void uibox::customdestroy()
	{
	}

	void uibox::LoadTex(const char* texloc, const char* texture)
	{
		tex = CtxName::ctx.Ren->Textures.Get2dTex(texloc, texture);

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