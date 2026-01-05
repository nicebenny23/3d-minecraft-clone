#include "../math/vector2.h"

#include "../util/dynamicarray.h"
#include <string>
#include "../math/geometry.h"
#include "ui.h"
#pragma once


extern stn::Id textarray;
void inittextarray();
const v2::Vec2 cubeuv[] = {
	v2::Vec2(0, 1),
	v2::Vec2(1, 1),
	v2::Vec2(0, 0),
	v2::Vec2(1, 0),
}; 
const v2::Vec2 offset[] = {
	v2::Vec2(-1,-1),
	v2::Vec2(1, -1),
	v2::Vec2(-1, 1),
	v2::Vec2(1, 1),

};
struct text_component:ecs::component
{	

	void set_handle();
	text_component(v2::Vec2 textcenter,float textscale);
	void write();
	renderer::RenderableHandle handle;
	std::string word;  
	template<typename... Args>
	void format(const std::format_string<Args...>& fmt, Args&&... args) {
		word = std::format(fmt, std::forward<Args>(args)...);
	}
	void recalculateword();
	void customdestroy();
	void render();

};



