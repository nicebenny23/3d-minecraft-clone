#include "liquid.h"

void com()
{
}
#include "liquid.h"
float liquidtick=.1;

void updateltick()
{
	liquidtick -= timename::dt;
	if (liquidtick<0)
	{
		liquidtick = .16;
	}
}
