#include "../util/sharedptr.h"
#include "ui.h"
#include "renderer.h"
#include "../util/dynamicarray.h"
#include "../math/vector2.h"
#include "../math/geometry.h"
#include "ui.h"
using namespace stn;
using namespace geometry;
using namespace Cptr;
#pragma once
namespace uiboxname {


	

	struct ui_image_component:ecs::component
	{
		ui_image_component() = default;
		renderer::RenderableHandle tex_handle;
		ui::ui_handle handle;
		void update();
		void disable(){
			tex_handle.disable();
			handle.disable();
		}
		void enable() {
			tex_handle.enable();
			handle.enable();
		}
		void destroy_hook();
		void LoadTex(const char* texloc, const char* texture);
		ui_image_component(const char* texloc, const char* TextureName, v2::Vec2 scl, v2::Vec2 position, float boxpriority);
	};
	
}

