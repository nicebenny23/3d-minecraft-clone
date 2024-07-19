#include "renderer.h"
#include "../util/dynamicarray.h"
#include "../util/vector2.h"
#include "texture.h"
#include "../util/geometry.h"
using namespace dynamicarray;
using namespace geometry;
#ifndef uirender_H
#define uirender_H
namespace uirender {


	struct uibox
	{
		uibox() = default;
		texture tex;
		bool shouldrender;
		geometry::Box2d box;
		int id;
		float priority;
		bool mouseonblock();
		void (*onclick) (v2::Vector2);
		uibox(uibox& toreplace);
		void destroy();
		uibox(const char* texloc, v2::Vector2 scl, v2::Vector2 position, float boxpriority);
	};
	uibox* newbox(const char* texloc, v2::Vector2 scl, v2::Vector2 position,float boxpriority);


	
	extern array<uibox*> uilist;
	void initrenderlist();
	void renderuilist();
}


#endif // !uirender_H
