#include "../util/vector2.h"
#include "inventoryblock.h"
#include "menu.h"
#ifndef itemstorage_HPP
#define itemstorage_HPP
extern int currentcontid;
void deleteallcontainerfiles();
const char* writefilename(int id);
struct Container
{
	short containerid;
	Container() {};
	void destroy() {

		for (int i = 0; i < databuf.length; i++)
		{
			databuf[i].destroyitem();
		}
		databuf.destroy();
	}
	Container(int newid);
	void writetofile();
	
	array<itemslot> databuf;
	itemslot& getlocalat(int xpos,int ypos) {
		return databuf[xpos + ypos * sizex];
	}
	itemslot& at(int ind) {
		return databuf[ind];
	}
	bool clicked() {
		if (!ismenuopen())
		{
			return false;
		}
		if (!userinput::mouseleft.pressed)
		{
			return false;
		}
		for (int i = 0; i < databuf.length; i++)
		{
			itemslot* slt = &databuf[i];
			if (slt->frame->mouseonblock()) {
				return true;
			}
		}
		return false;
	}
	void deletebelowzero() {


		for (int i = 0; i < databuf.length; i++)
		{
			if (databuf[i].helditem!=nullptr)
			{
				if(databuf[i].helditem->amt<=0){
				
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

				databuf[i].giveitem(elemid,0);
				
				databuf[i].helditem->give(amt);
				if (databuf[i].helditem->amt==0)
				{

					databuf[i].destroyitem();
				}
				
		
				continue;
			}
		
			if (databuf[i].helditem->id == elemid) {

				databuf[i].helditem->give(amt);
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

		containerid = currentcontid;
		currentcontid++;
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
		setviewable(false);
	}
	void update() {


		for (int i = 0; i < databuf.length; i++)
		{
			

			databuf[i].updatestate();
		}
	
	}
	
};


#endif // !itemstorage_H
