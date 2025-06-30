#pragma once
#include "../util/sharedptr.h"
#include "../util/dynamicarray.h"
 
// base ui objects like boxes and text,used by more advanced ui objects including decals//buttons
struct uiobj
{
	virtual void destroy();
	bool enabled;
	int id;
	virtual void render();
};
//not needed
//dynamicarray::array<Cptr::cptr<uiobj>> uiarray;

 // !baseui_HPP
