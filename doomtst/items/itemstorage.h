#include "../util/vector2.h"
#include "inventoryblock.h"

#ifndef itemstorage_HPP
#define itemstorage_HPP
struct Container
{
	Container() {};
	
	array<itemslot> databuf;
	itemslot& getlocalat(int xpos,int ypos) {
		return databuf[xpos + ypos * sizex];
	}
	itemslot& getlocalat(int ind) {
		return databuf[ind];
	}
	void deletebelowzero() {


		for (int i = 0; i < databuf.length; i++)
		{
			if (databuf[i].helditem!=nullptr)
			{
				if(databuf[i].helditem->amt<0){
				
					databuf[i].destroyitem();
				}
			}
		}
	}
	bool fill(int elemid, int amt) {


		for (int i = 0; i < databuf.length; i++)
		{
			if (amt<=0)
			{
				break;
			}
			if (databuf[i].helditem == nullptr)
			{

				databuf[i].giveitem(elemid);
				databuf[i].setviewable(false);
				int putamt = Min(amt, databuf[i].helditem->maxamt);
				if (0 < putamt)
				{

					databuf[i].helditem->amt = putamt;
					amt -= putamt;
				}
				else
				{
					databuf[i].destroyitem();
				}
				continue;
			}
		
			if (databuf[i].helditem->id == elemid) {

				int putamt =Min(amt, databuf[i].helditem->maxamt - databuf[i].helditem->amt);
				if (0<putamt)
				{

					databuf[i].helditem->amt+=putamt;
					amt -= putamt;
				}

			}
		
		
		}
		if (amt==0)
		{
			return true;
		}
		return false;
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
	//void addtocontainer(int id);
	Container(int xsize, int ysize,float xoff,float yoff) {


		sizex = xsize;
		sizey = ysize;
		offset = v2::Vector2(xoff, yoff);
		int ind = 0;
		for (int i = 0; i < xsize; i++)
		{
			for (int j =0; j < ysize; j++)
			{

				databuf[ind] = itemslot(i+xoff-float(sizex)/2.0f, j+yoff-ysize / 2.0f);
				ind++;
			}
		}
	}
	void testmouseclick() {


		for (int i = 0; i < databuf.length; i++)
		{
			

				databuf[i].testclick();
			
		}
	}
	
};


#endif // !itemstorage_H
