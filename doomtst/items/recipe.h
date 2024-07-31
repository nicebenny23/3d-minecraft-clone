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
	bool isauto = false;
	float timetillcraft;
	float timetocraft;
	extrarecipeattributes() {
		timetillcraft = 0;
		isauto = false;
		timetocraft = 0;
	}
	extrarecipeattributes(bool isreversable, float craftingtime) {
		timetillcraft = 0;
		isauto = !isreversable;
		timetocraft = craftingtime;
	}
};
struct managerstate {
	bool itemcreatedinside;
	bool cancraft;
	bool enabled;
	bool craftedthisframe;
	
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
	
	autocraftstatetype autocraft();
		irecipe* searchrecipe();
	array<irecipe>* recipelist;
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
#endif