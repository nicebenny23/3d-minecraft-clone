#include "../util/dynamicarray.h"
#include "Container.h"
#include "../util/fileloader.h"
using namespace dynamicarray;
#ifndef recipe_hpp
#define recipe_hpp
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
	int ysize;
	iteminrecipe itemcreated;
	bool cancraft(Container* resourcecont);
	irecipe(iteminrecipe* itemarray,iteminrecipe created,int sizex,int sizey);

	array<iteminrecipe> recipe;
	};
struct extrarecipeattributes {

	bool reversable;
	float timetocraft;
	extrarecipeattributes() {

		reversable = true;
		timetocraft = 0;
	}
	extrarecipeattributes(bool isreversable, float craftingtime) {

		reversable = isreversable;
		timetocraft = craftingtime;
	}
};
struct managerstate {
	
	bool cancraft;
	bool enabled;
	bool craftedthisframe;
	managerstate() {
		cancraft = true;
		enabled = false;
		craftedthisframe = false;
	}
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
	void ontake();
	void preview(); 
	void craft();
	irecipe* searchrecipe();
	array<irecipe>* recipelist;
	void save();
	void enable();
	void disable();
	unsigned int getcombinedid() {
		return 256 * newitemlocation->containerid + resourcecontainer->containerid;
	}
	recipemanager(const char* filename, int sizex, int sizey);
	recipemanager() {

		newitemlocation = nullptr;
		resourcecontainer = nullptr;
	}


};
#endif