#include "inventoryblock.h"
#include "../util/mathutil.h"
void destroyonclick(invblock& toremove) {
	toremove.held.itemsprite->shouldrender = false;

}
invblock::invblock(int xloc,int yloc)
{
	float xval = ((2*xloc+.5) / float(xamt));
	float yval = ((2 * yloc+ 1) / float(yamt)) ;
	v2::Vector2 scale = v2::Vector2(1 / float(xamt),1/float(yamt));
	framebox= uirender::newbox("images\\blockholder.png",scale, v2::Vector2(xval,yval),-130);
	held = item(1);
	held.itemsprite->bx.center = v2::Vector2(xval, yval);
	held.itemsprite->bx.scale= scale/1.4f;
	onclick = destroyonclick;
}

invblock::invblock(int xloc, int yloc, void(*clickaction)(invblock&))
{
	
	float xval = ((2 * xloc + .5) / float(xamt)) ;
	float yval = ((2 * yloc + 1) / float(yamt)) ;
	v2::Vector2 scale = v2::Vector2(1 / float(xamt), 1 / float(yamt));
	framebox = uirender::newbox("images\\blockholder.png", scale, v2::Vector2(xval, yval), -130);
	held = item(1);
	held.itemsprite->bx.center = v2::Vector2(xval, yval);
	held.itemsprite->bx.scale = scale / 1.4f;

	onclick = clickaction;
}

void invblock::giveitem(int id)
{
	if (!empty)
	{
	
		Assert("cant give item to unempty array");
	}
	held = item(id);
}

void invblock::transferitem(item* otherholder)
{
	
		if (otherholder->state==beingheld)
		{
			otherholder->itemsprite->bx.center = held.itemsprite->bx.center;
			std::swap(otherholder->itemsprite, held.itemsprite);
			std::swap(otherholder->id, held.id);
			
			std::swap(otherholder->state, held.state);
		}
	
}

void invblock::destroyitem()
{


}
