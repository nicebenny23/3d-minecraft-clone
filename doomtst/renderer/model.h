#include "mesh.h"
#include "../game/gameobject.h"
#include "texture.h"
#include "../game/transform.h"
#ifndef model_HPP
#define model_HPP
enum  meshconnecttype
{
	normalmeshconnect = 0,
	wormmeshconnect = 1,
};
struct model:gameobject::component
{
	meshname::mesh& operator[](int index);

	meshconnecttype connectiontype;
	void draw();
	void add(const char* meshname, const char* meshtexname,Vector3 pos=zerov);
	model(meshconnecttype connectmethod = normalmeshconnect);
	void renderupdate();

	array<meshname::mesh*> meshlist;
	void destroy();
	void start();
};
extern array<model> modellist;

#endif // !modelname
