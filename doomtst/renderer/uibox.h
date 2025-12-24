#include "../util/sharedptr.h"
#include "ui.h"
#include "renderer.h"
#include "../util/dynamicarray.h"
#include "../math/vector2.h"
#include "../math/geometry.h"
using namespace stn;
using namespace geometry;
using namespace Cptr;
#pragma once
namespace uiboxname {


	

	struct uibox:ui::uielement
	{
		uibox() = default;
		renderer::RenderableHandle tex_handle;
		geometry::Box2d box;
		void update();
		bool mouseonblock();
		uibox(uibox& toreplace);
		void render();
		void customdestroy();
		void LoadTex(const char* texloc, const char* texture);
		uibox(const char* texloc, const char* TextureName, v2::Vec2 scl, v2::Vec2 position, float boxpriority);
	};
	
}

