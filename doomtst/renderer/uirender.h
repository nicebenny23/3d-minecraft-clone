#include "renderer.h"
#include "../util/dynamicarray.h"
#include "../util/vector2.h"
#include "texture.h"
using namespace dynamicarray;

#ifndef uirender_H
#define uirender_H
namespace uirender {


	struct uibox
	{
		texture tex;
		bool shouldrender;
		v2::Vector2 pos;
		v2::Vector2 scale;
		int id;


		uibox(const char* texloc, v2::Vector2 scl, v2::Vector2 position);
	};
	uibox* newbox(const char* texloc, v2::Vector2 scl, v2::Vector2 position);
	
	
	
	extern array<uibox*> uilist;
	void initrenderlist();
	void renderuilist();
}


#endif // !uirender_H
