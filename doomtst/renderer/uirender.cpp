#include "uirender.h"
using namespace uirender;
array<uirender::uibox*> uirender::uilist;
uirender::uibox::uibox(const char* texloc, v2::Vector2 scl, v2::Vector2 ps)
{

	tex = texture(texloc,png);
	scale = scl;
	ps = pos;
	for (int i = 0; i < 100; i++)
	{
		if (uilist[i] == nullptr) {
			uilist[i] = this;
			id = i;
			return;
		}
	}
	Assert("uilist full");
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
	indbuf[2] = 2;
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
			
			uilist[i]->tex.apply();
			v2::Vector2 min = uilist[i]->pos - uilist[i]->scale;
			v2::Vector2 max = uilist[i]->pos + uilist[i]->scale;
	
			databuf.append(min.x);
			databuf.append(min.y);
			databuf.append(0);
			databuf.append(0);
			databuf.append(min.x);
			databuf.append(max.y);
			databuf.append(0);
			databuf.append(1);
			databuf.append(max.x);
			databuf.append(max.y);
			databuf.append(1);
			databuf.append(1);
			databuf.append(max.x);
			databuf.append(min.y);
			databuf.append(1);
			databuf.append(0);
			renderer::render2dquadlist(Voa, ibo, VBO, databuf, indbuf);
			databuf.destroy();
		}
	}
	Voa.destroy();
	VBO.destroy();
	ibo.destroy();
}
