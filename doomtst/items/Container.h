#include "../util/vector2.h"
#include "itemslot.h"
#include "menu.h"
#pragma once 
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
			databuf[i].framedecal->destroy();

			databuf[i].framedecal->destroy();
		}
		databuf.destroy();
	}
	Container(int newid);
	void writetofile();
	bool enabled;
	
	array<itemslot,true> databuf;
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
		if (!CtxName::ctx.Inp->mouseleft().pressed)
		{
			return false;
		}
		for (int i = 0; i < databuf.length; i++)
		{
			itemslot* slt = &databuf[i];
			if (slt->framedecal->mouseonblock()) {
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
	bool fill(int elemid, int& amt,bool createnew) {


		for (int i = 0; i < databuf.length; i++)
		{
			if (amt<=0)
			{
				break;
			}
			


				if (databuf[i].helditem == nullptr)
				{
					if (createnew)
					{
						databuf[i].giveitem(elemid, 0);

						databuf[i].helditem->give(amt);
						if (databuf[i].helditem->amt == 0)
						{

							databuf[i].destroyitem();
						}
					}

					continue;
				}
			
			if (databuf[i].helditem->id == elemid) {
				if (databuf[i].helditem->itemtype == count)
				{


					databuf[i].helditem->give(amt);
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
	void enable() {
		for (int i = 0; i < databuf.length; i++)
		{
			databuf[i].enable();
		}

		enabled = true;

	}
	void disable() {

		for (int i = 0; i < databuf.length; i++)
		{
			databuf[i].disable();
		}
		enabled=false;

	}
	//void addtocontainer(int id);
	Container(int xsize, int ysize,float xoff,float yoff) {

		containerid = currentcontid;
		currentcontid++;
		sizex = xsize;
		sizey = ysize;
		offset = v2::Vector2(xoff, yoff);
		int ind = 0;
		databuf = array<itemslot,true>(xsize * ysize);
		for (int j = 0; j < ysize; j++)
		{
			for (int i = 0; i < xsize; i++)
			{


				databuf[ind] = itemslot(i + xoff - float(sizex) / 2.0f, j + yoff - ysize / 2.0f);
				ind++;

			}
		}
		disable();
	}
	void update() 
	{
		if (enabled)
		{



			for (int i = 0; i < databuf.length; i++)
			{


				databuf[i].updatestate();
			}

		}
	}
};


 // !itemstorage_H
