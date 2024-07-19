#include "../util/dynamicarray.h"
#include "itemstorage.h"
using namespace dynamicarray;
#ifndef recipe_hpp
#define recipe_hpp
struct iteminrecipe {

	int id;
	int amt;

};

struct irecipe {
	int xsize;
	int ysize;
	iteminrecipe itemcreated;
	bool cancraft(Container* resourcecont);

	iteminrecipe craft(Container* resourcecont);
	iteminrecipe* recipe;
	};
struct recipemanager {
	int xsize;
	int ysize;
	Container* newitemlocation;
	Container* resourcecontainer;
	void destroy();
	recipemanager(int orginxsize,int orginysize,array<irecipe>& recipes);
	
	void ontake();
	void preview();
	irecipe* searchrecipe();
	array<irecipe>& recipelist;



};
#endif