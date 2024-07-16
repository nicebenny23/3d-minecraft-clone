#include "uirender.h"
using namespace uirender;
array<uirender::uibox*> uirender::uilist;
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
uirender::uibox::uibox(const char* texloc, v2::Vector2 scl, v2::Vector2 position)
{

	tex = texture(texloc,png);
	scale = scl;
	pos = position;
	shouldrender = true;
}

void uirender::initrenderlist()
{
	uilist = array<uibox*>();
	for (int i = 0; i < 100; i++)
	{
		uilist[i] = nullptr;
	}
}

void uirender::renderuilist()
{
	
	array<unsigned int> indbuf = array<unsigned int>();
	indbuf[0] = 0;
	indbuf[1] = 1;
	indbuf[2] = 3;
	indbuf[3] = 0;
	indbuf[4] = 3;
	indbuf[5] = 2;
	renderer::changerendertype(renderer::renderui);
	
	vao Voa;
	vbuf VBO;
	vbuf ibo;
	Voa.generate();
	VBO.generate(GL_ARRAY_BUFFER);
	ibo.generate(GL_ELEMENT_ARRAY_BUFFER);
	for (int i = 0; i < 100; i++)
	{

		if (uilist[i]!=nullptr&&uilist[i]->shouldrender)
		{
			array<float> databuf = array<float>();
			texture uitexture = uilist[i]->tex;
			uitexture.apply();
		
			for (int j = 0; j< 4; j++)
			{
				v2::Vector2 pos = uilist[i]->pos + offset[j] * (uilist[i]->scale);
				databuf.append(pos.x);
				databuf.append(pos.y);
				databuf.append(cubeuv[2 * j]);
				databuf.append(cubeuv[2 * j+1]);
			}
			
		
			renderer::render2dquadlist(Voa, ibo, VBO, databuf, indbuf);
			databuf.destroy();
		}
	}
	indbuf.destroy();
	Voa.destroy();
	VBO.destroy();
	ibo.destroy();
}

uibox* uirender::newbox(const char* texloc, v2::Vector2 scl, v2::Vector2 position)
{
	uibox* newuibox = new uibox(texloc, scl,position);
	for (int i = 0; i < 100; i++)
	{
		if (uilist[i] == nullptr) {
			uilist[i] =newuibox;
			newuibox->id = i;
			return newuibox;
		}
		
	}
	return nullptr;
}
