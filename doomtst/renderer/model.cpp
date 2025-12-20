#pragma once
#include "model.h"
#include "../game/GameContext.h"

ModelMeshName::ModelMesh& model::operator[](int index)
{
	return meshlist[index];
}
void model::draw()
{
}
void model::add(const char* meshname,const char * meshtexname)
{
	meshlist.push(ModelMeshName::loadmesh(meshtexname,meshname, zerov));
}
model::model(meshconnecttype connectmethod )
{
	//utype = ecs::updaterender;
	connectiontype = connectmethod;
	meshlist = array<ModelMeshName::ModelMesh>();
}

void model::update()
{
	v3::Point3 how = owner().get_component<ecs::transform_comp>().transform.position;
	glm::mat4* model =new glm::mat4((owner().get_component<ecs::transform_comp>().transform.ToMatrix()));
	for (int i = 0; i < meshlist.length(); i++)
	{
		meshlist[i].modelmatrix =model;
		ModelMeshName::rendermesh(meshlist[i]);
	}
	delete model;
}

void model::destroy()
{
	for (int i = 0; i < meshlist.length(); i++)
	{
		meshlist[i].destroy();
	}
}

void model::start()
{
	
}
