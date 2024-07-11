#include "uirender.h"
using namespace uirender;
array<uirender::uibox*> uirender::uilist;
const float cubeuv[] = {
	0, 0,
	1, 0,

0, 1,
1, 1

};
const v2::Vector2 offset[] = {
	v2::Vector2(-1,-1),
	v2::Vector2(1, -1),

v2::Vector2(-1, 1),
v2::Vector2(1, 1)

};
uirender::uibox::uibox(const char* texloc, v2::Vector2 scl, v2::Vector2 ps)
{

	tex = texture(texloc,png);
	scale = scl;
	ps = pos;

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
	renderer::changerendertype(renderer::ui);
	
	vao Voa;
	vbuf VBO;
	vbuf ibo;
	Voa.generate();
	VBO.generate(GL_ARRAY_BUFFER);
	ibo.generate(GL_ELEMENT_ARRAY_BUFFER);
	for (int i = 0; i < 100; i++)
	{

		if (uilist[i]!=nullptr)
		{
			array<float> databuf = array<float>();
			texture te = uilist[i]->tex;
			te.apply();
			
			v2::Vector2 min =  uilist[i]->scale;
			v2::Vector2 max = uilist[i]->pos + uilist[i]->scale;
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

uibox* uirender::newbox(const char* texloc, v2::Vector2 scl, v2::Vector2 ps)
{
	uibox* bx = new uibox(texloc, scl, ps);
	for (int i = 0; i < 100; i++)
	{
		if (uilist[i] == nullptr) {
			uilist[i] = bx;
			bx->id = i;
			return bx;
		}
		return nullptr;
	}
}
