#include "mesh.h"
#include "../game/gameobject.h"
#include "texture.h"
#include "../game/transform.h"
#ifndef model_HPP
#define model_HPP

struct model:gameobject::component
{
	meshname::mesh& operator[](int index);
	
	void draw();
	void add(const char* meshname, const char* meshtexname);
	model();
	void renderupdate();
	array<meshname::mesh*> meshlist;
	void destroy();
	void start();
};
extern array<model> modellist;

#endif // !modelname
