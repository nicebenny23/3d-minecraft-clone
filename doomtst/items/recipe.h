#include "../util/dynamicarray.h"
#include "itemstorage.h"
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
struct recipemanager {
	int xsize;
	int ysize;
	Container* newitemlocation;
	Container* resourcecontainer;
	void destroy();
	void createcontainers();
	void addrecipe(irecipe recipe);
	void testmouseclick();
	void ontake();
	void preview(); 
	void craft();
	irecipe* searchrecipe();
	array<irecipe>* recipelist;
	void setviewable(bool state);
	recipemanager(const char* filename, int sizex, int sizey);
	recipemanager() {

		newitemlocation = nullptr;
		resourcecontainer = nullptr;
	}


};
#endif