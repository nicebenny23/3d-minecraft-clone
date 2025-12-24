#pragma once
#include "liquid.h"
#include "../game/time.h"
#include "../game/ecs/query.h"
void com()
{
}
float liquidtick=.1;

void updateltick()
{
	liquidtick -= CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().dt;
	if (liquidtick<0)
	{
		liquidtick = .16;
	}
}
