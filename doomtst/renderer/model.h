#pragma once
#include "../game/objecthelper.h"
#include "ModelMesh.h"
#include "../game/ecs/game_object.h"
#include "../game/transform.h"

enum  meshconnecttype
{
	normalmeshconnect = 0,
	wormmeshconnect = 1,
};
struct model: ecs::component
{
	ModelMeshName::ModelMesh& operator[](int index);

	meshconnecttype connectiontype;
	void draw();
	void add(const char* meshname, const char* meshtexname,Vec3 pos=zerov);
	model(meshconnecttype connectmethod = normalmeshconnect);
	void update();
	
	array<ModelMeshName::ModelMesh> meshlist;

	using iterator = typename stn::array<ModelMeshName::ModelMesh>::iterator;
	using const_iterator = typename stn::array<ModelMeshName::ModelMesh>::const_iterator;
	iterator begin() { return meshlist.begin(); }
	iterator end() { return meshlist.end(); }
	const_iterator begin() const { return meshlist.begin(); }
	const_iterator end()   const { return meshlist.end(); }
	void destroy();
	void start();
	void setmodelshader();
};
extern array<model> modellist;
