#include "ui.h"
#include <glm/common.hpp>
array<cptr<ui::uielement>> ui::uielemlist;

void ui::createuilist()
{
	uielemlist = array<cptr<uielement>>(uisize,true);
	for (int i = 0; i < uisize; i++)
	{
		uielemlist[i] = cptr<uielement> ();
	}
}

int ui::compareuielem(const void* b, const void* a)
{
	return glm::sign(uielemlist[*(int*)b]->priority - uielemlist[*(int*)a]->priority);
}

void ui::uielement::customdestroy()
{
}

void ui::uielement::destroy()
{
	uielemlist[id] = cptr<uielement>(nullptr);
	customdestroy();
}
