#include "../util/vector2.h"
#include "texture.h"
#include "../util/dynamicarray.h"
#include <string>
#include "../util/geometry.h"
#include "ui.h"
using namespace Cont;
using namespace geometry;
#ifndef  textrender_HPP
#define textrender_HPP

extern TextureArray* textarray;
void inittextarray();
const v2::Vector2 cubeuv[] = {
	v2::Vector2(0, 1),


	v2::Vector2(1, 1),

	v2::Vector2(0, 0),
	v2::Vector2(1, 0),



	




	

}; const v2::Vector2 offset[] = {
	v2::Vector2(-1,-1),
	v2::Vector2(1, -1),

v2::Vector2(-1, 1),
v2::Vector2(1, 1)

};
struct integertext:ui::uielement
{	float scale;
	v2::Vector2 center;
	
	integertext(v2::Vector2 textcenter,float textscale);
	void write();
	
	int value;
	std::string word;
		void recalculateword();
	void customdestroy();
	void render();

};

#endif // ! textrender_HPP
