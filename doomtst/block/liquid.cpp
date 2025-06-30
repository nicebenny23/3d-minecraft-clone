#pragma once
#include "liquid.h"
#include "../util/time.h"
void com()
{
}
float liquidtick=.1;

void updateltick()
{
	liquidtick -= CtxName::ctx.Time->dt;
	if (liquidtick<0)
	{
		liquidtick = .16;
	}
}
