#include "uirender.h"
#include "../util/geometry.h"
#include "../util/userinput.h"
#include "../util/mathutil.h"
using namespace uirender;
constexpr int listsize= 400;
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
bool uirender::uibox::mouseonblock()
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

uirender::uibox::uibox(uibox& toreplace)
{
	Assert("cant copy ui box");
}

void uirender::uibox::destroy()
{
	uilist[id] = nullptr;
	tex.destroy();

}

uirender::uibox::uibox(const char* texloc, v2::Vector2 scl, v2::Vector2 position, float boxpriority)
{
	priority = boxpriority;
	tex = texture(texloc,png);
	box.scale = scl;
	box.center = position;
	shouldrender = true;
}

void uirender::initrenderlist()
{
	uilist = array<uibox*>();
	for (int i = 0; i < listsize; i++)
	{
		uilist[i] = nullptr;
	}
}

int compareui(const void* b, const void* a) {
	return sign(uilist[*(int*)b]->priority- uilist[*(int*)a]->priority);
}
void uirender::renderuilist()
{
	array<int> indicetosort;
	for (int i = 0; i < listsize; i++)
	{
		if (uilist[i]!=nullptr)
		{

			indicetosort.append(uilist[i]->id);
		}
	}
	std::qsort(indicetosort.getdata(), indicetosort.length, sizeof(int), compareui);
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
	for (int ind = 0; ind< indicetosort.length; ind++)
	{
		int i = indicetosort[ind];
		if (uilist[i]!=nullptr&&uilist[i]->shouldrender)
		{
			array<float> databuf = array<float>();
			texture uitexture = uilist[i]->tex;
			uitexture.apply();
		
			for (int j = 0; j< 4; j++)
			{
				v2::Vector2 pos = uilist[i]->box.center + offset[j] * (uilist[i]->box.scale);
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

uibox* uirender::newbox(const char* texloc, v2::Vector2 scl, v2::Vector2 position, float boxpriority)
{
	
	uibox* newuibox = new uibox(texloc, scl,position,boxpriority);
	for (int i = 0; i < listsize; i++)
	{
		if (uilist[i] == nullptr) {
			uilist[i] =newuibox;
			newuibox->id = i;
			return newuibox;
		}
		
	}
	Assert("not enogh room to inat aanewbox");
	return nullptr;
}
