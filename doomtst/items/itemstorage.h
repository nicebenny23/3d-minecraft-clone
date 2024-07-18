#include "../util/vector2.h"
#include "inventoryblock.h"
#ifndef itemstorage_HPP
#define itemstorage_HPP
struct Container
{
	Container() {};
	
	array<invblock> databuf;
	invblock& getlocalat(int xpos,int ypos) {
		return databuf[xpos + ypos * sizex];
	}
	int sizex;
	int sizey;
	v2::Vector2 offset;
	void setviewable(bool isviewable) {
		for (int i = 0; i < databuf.length; i++)
		{
			databuf[i].setviewable(isviewable);
		}

	
	}
	Container(int xsize, int ysize,float xoff,float yoff) {


		sizex = xsize;
		sizey = ysize;
		offset = v2::Vector2(xoff, yoff);
		int ind = 0;
		for (int i = 0; i < xsize; i++)
		{
			for (int j =0; j < ysize; j++)
			{

				databuf[ind] = invblock(i+xoff-sizex/2.0f, j+yoff-ysize / 2.0f);
				ind++;
			}
		}
	}
	
};


#endif // !itemstorage_H
