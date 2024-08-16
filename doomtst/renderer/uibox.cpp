#include "uibox.h"
#include "../util/geometry.h"
#include "../util/userinput.h"
#include "../util/mathutil.h"
using namespace uiboxname;
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
void uiboxname::uibox::update()
{
	state.rightclicked = false;
	state.hovered = false;
	state.leftclicked = false;
	if (mouseonblock())
	{
		state.hovered = true;
		if (userinput::mouseleft.pressed)
		{
			state.leftclicked = true;
		}
		if (userinput::mouseright.pressed)
		{
			state.rightclicked = true;
		}
	}
}
bool uiboxname::uibox::mouseonblock()
{
	v2::Vector2 normedpos = userinput::normedmousepos;
	normedpos -= box.center;
	if ( abs(normedpos.x)<box.scale.x)
	{
		if (abs(normedpos.y) < box.scale.y)
		{
			return true;
		}
	}
	return false;
}

uiboxname::uibox::uibox(uibox& toreplace)
{
	id = toreplace.id;
	tex = toreplace.tex;
	box = toreplace.box;
	
	ui::uielemlist[id].cset<uibox>(* new cptr<uibox>(this));
	priority = toreplace.priority;
	state.enabled = toreplace.state.enabled;

	toreplace.destroy();
}

void uiboxname::uibox::render()
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

		vao Voa;
		vbuf VBO;
		vbuf ibo;
		Voa.generate();
		VBO.generate(GL_ARRAY_BUFFER);
		ibo.generate(GL_ELEMENT_ARRAY_BUFFER);

		array<float> databuf = array<float>();
		texture uitexture = tex;
		uitexture.apply();

		for (int j = 0; j < 4; j++)
		{
			v2::Vector2 pos = box.center + offset[j] * (box.scale);
			databuf.append(pos.x);
			databuf.append(pos.y);
			databuf.append(cubeuv[2 * j]);
			databuf.append(cubeuv[2 * j + 1]);
		}


		renderer::render2dquadlist(Voa, ibo, VBO, databuf, indbuf);
		databuf.destroy();
		indbuf.destroy();
		Voa.destroy();
		VBO.destroy();
		ibo.destroy();
	}
	
}



void uiboxname::uibox::customdestroy()
{
	tex.destroy();
}

uiboxname::uibox::uibox(const char* texloc, v2::Vector2 scl, v2::Vector2 position, float boxpriority)
{
	priority = boxpriority;
	tex = texture(texloc);
	box.scale = scl;
	box.center = position;
	state.enabled = true;
}
