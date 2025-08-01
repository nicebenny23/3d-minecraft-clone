#include "../util/dynamicarray.h"
#include "Container.h"
#include "../util/fileloader.h"
using namespace stn;
#pragma once 
struct iteminrecipe {

	int id;
	int amt;
	iteminrecipe(int itemid, int itemamt) {
		id = itemid;
		amt = itemamt;
    }
	iteminrecipe() {
		id = -1;
		amt = -1;
	}
};

struct irecipe {
	int xsize;
	irecipe() {

				xsize = -1;
		ysize = -1;
		
	}
	int ysize;
	iteminrecipe itemcreated;
	bool cancraft(Container* resourcecont,bool exact=false);
	irecipe(iteminrecipe* itemarray,iteminrecipe created,int sizex,int sizey);

	array<iteminrecipe> recipe;
	};
struct extrarecipeattributes {
	bool isauto = false;
	float timetillcraft;
	float timetocraft;
	extrarecipeattributes() {
		timetillcraft = 0;
		isauto = false;
		timetocraft = 0;
	}
	extrarecipeattributes(bool doneauto, float craftingtime) {
		timetillcraft = craftingtime;
		isauto = doneauto;
		timetocraft = craftingtime;
	}
};
struct managerstate {
	bool itemcreatedinside;
	bool cancraft;
	bool enabled;
	bool craftedthisframe;
	bool shouldmax;
	iteminrecipe crafted;
	managerstate() {
		
		cancraft = true;
		enabled = false;
		craftedthisframe = false;
	}
};
enum autocraftstatetype
{
	crafted=0,
	iscrafting=1,
	notcrafting=2,
};
struct recipemanager {
	int xsize;
	int ysize;

	managerstate state;
	Container* newitemlocation;
	Container* resourcecontainer;
	extrarecipeattributes attributes;
	void destroy();
	void createcontainers();
	void addrecipe(irecipe recipe);
	void updatestate();
	void autoupdate();
	
	void preview(); 
	void craft();
	bool isitempreview();
	int timecancraft();
	autocraftstatetype autocraft();
		irecipe* searchrecipe();
	array<irecipe> recipelist;
	irecipe* currecipe;
	void save();
	void enable();
	void disable();
	bool previewvalid();
	unsigned int getcombinedid() {
		return 256 * newitemlocation->containerid + resourcecontainer->containerid;
	}
	recipemanager(const char* filename, int sizex, int sizey);
	recipemanager() {
		currecipe = nullptr;
		newitemlocation = nullptr;
		resourcecontainer = nullptr;
	}


};
