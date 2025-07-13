#pragma once
#include "ui.h"
#include "../util/mathutil.h"
array<cptr<ui::uielement>,true> ui::uielemlist;

void ui::createuilist()
{
	uielemlist = array<cptr<uielement>,true>(uisize);

}

int ui::compareuielem(const void* b, const void* a)
{
	return z_sign(uielemlist[*(int*)b]->priority - uielemlist[*(int*)a]->priority);
}

void ui::uielement::customdestroy()
{
}

void ui::uielement::destroy()
{
	uielemlist[id] = cptr<uielement>(nullptr);
	customdestroy();
}
