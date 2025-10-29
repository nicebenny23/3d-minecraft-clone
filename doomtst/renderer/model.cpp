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
void model::add(const char* meshname,const char * meshtexname,Vec3 pos)
{
	meshlist.push(ModelMeshName::loadmesh(meshtexname,meshname, zerov));
meshlist.last().transform.position = pos;
}
model::model(meshconnecttype connectmethod )
{
	utype = gameobject::updaterender;
	connectiontype = connectmethod;
	meshlist = array<ModelMeshName::ModelMesh>();
}

void model::update()
{
	v3::Vec3 how = owner.transform().position;
	glm::mat4* model =new glm::mat4((owner.transform().ToMatrix()));
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

void model::setmodelshader()
{

	//toto readd CtxName::ctx.Ren->context.Bind(CtxName::ctx.Ren->Shaders["ModelShader"]);
	CtxName::ctx.Ren->SetType("Model");
	
}
