#include "../util/vector2.h"
#include "texture.h"
#include "../util/dynamicarray.h"
#include <string>
#include "../util/geometry.h"
#include "ui.h"
#pragma once


extern Ids::Id textarray;
void inittextarray();
const v2::Vec2 cubeuv[] = {
	v2::Vec2(0, 1),


	v2::Vec2(1, 1),

	v2::Vec2(0, 0),
	v2::Vec2(1, 0),



	




	

}; const v2::Vec2 offset[] = {
	v2::Vec2(-1,-1),
	v2::Vec2(1, -1),

v2::Vec2(-1, 1),
v2::Vec2(1, 1)

};
struct integertext:ui::uielement
{	float scale;
	v2::Vec2 center;
	void set_handle();
	integertext(v2::Vec2 textcenter,float textscale);
	void write();
	renderer::RenderableHandle handle;
	int value;
	std::string word;
		void recalculateword();
	void customdestroy();
	void render();

};

