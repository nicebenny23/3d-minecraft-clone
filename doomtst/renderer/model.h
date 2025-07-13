#pragma once
#include "../game/objecthelper.h"
#include "ModelMesh.h"
#include "../game/gameobject.h"
#include "texture.h"
#include "../game/transform.h"

enum  meshconnecttype
{
	normalmeshconnect = 0,
	wormmeshconnect = 1,
};
struct model: gameobject::component
{
	ModelMeshName::ModelMesh& operator[](int index);

	meshconnecttype connectiontype;
	void draw();
	void add(const char* meshname, const char* meshtexname,Vec3 pos=zerov);
	model(meshconnecttype connectmethod = normalmeshconnect);
	void update();

	array<ModelMeshName::ModelMesh*> meshlist;
	void destroy();
	void start();
	void setmodelshader();
};
extern array<model> modellist;
