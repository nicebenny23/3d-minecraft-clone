#include "../util/vector2.h"
#include "texture.h"
#include "../util/dynamicarray.h"
#include <string>
#include "../util/geometry.h"
using namespace dynamicarray;
using namespace geometry;
#ifndef  textrender_HPP
#define textrender_HPP
#define maxtextamount 100
extern texturearray textarray;
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
struct integertext
{
	bool shouldrender;
	float scale;
	v2::Vector2 center;
	
	integertext(v2::Vector2 textcenter,float textscale);
	void write();
	
	int value;
	std::string word;
		void recalculateword();
	void destroy();
	

	int id;
};
integertext* createinteger(v2::Vector2 textcenter, float textscale);

void rendertextlist();
extern array<integertext*> textlist;
inline void inittextlist() {
	
	textlist = array<integertext*>(maxtextamount);
	for (int i = 0; i < textlist.length; i++)
	{
		textlist[i] = nullptr;
	}
}
;
#endif // ! textrender_HPP
