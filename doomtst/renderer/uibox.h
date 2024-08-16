#include "renderer.h"
#include "../util/dynamicarray.h"
#include "../util/vector2.h"
#include "texture.h"
#include "../util/sharedptr.h"

#include "ui.h"
#include "../util/geometry.h"
using namespace dynamicarray;
using namespace geometry;
using namespace Cptr;
#ifndef uirender_H
#define uirender_H
namespace uiboxname {


	

	struct uibox:ui::uielement
	{
		uibox() = default;
		texture tex;
		geometry::Box2d box;
		void update();
		bool mouseonblock();
		uibox(uibox& toreplace);
		void render();
		void customdestroy();
		uibox(const char* texloc, v2::Vector2 scl, v2::Vector2 position, float boxpriority);
	};

}


#endif // !

