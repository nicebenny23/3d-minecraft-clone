#pragma once
#include "model.h"
#include "../game/GameContext.h"

ModelMeshName::ModelMesh& model::operator[](int index)
{
	return *meshlist[index];
}
void model::draw()
{
}
void model::add(const char* meshname,const char * meshtexname,Vector3 pos)
{
		Texture2D* meshtex = CtxName::ctx.Ren->Textures.Get2dTex(meshtexname,meshtexname);
	meshlist.push(ModelMeshName::loadmesh(meshname, meshtex, zerov));
meshlist[meshlist.length - 1]->transform.position = pos;
}
model::model(meshconnecttype connectmethod )
{
	utype = gameobject::updaterender;
	connectiontype = connectmethod;
	meshlist = array<ModelMeshName::ModelMesh*>();
}

void model::update()
{
	CtxName::ctx.Ren->SetType("Model");
	glm::mat4* model =new glm::mat4((owner.transform().ToMatrix()));
	for (int i = 0; i < meshlist.length; i++)
	{
		meshlist[i]->modelmatrix =model;
		ModelMeshName::rendermesh(meshlist[i]);
	}
	delete model;
}

void model::destroy()
{
	for (int i = 0; i < meshlist.length; i++)
	{
		meshlist[i]->destroy();
	}
}

void model::start()
{
	
}

void model::setmodelshader()
{

	CtxName::ctx.Ren->context.Bind(CtxName::ctx.Ren->Shaders["ModelShader"]);
	CtxName::ctx.Ren->SetType("Model");
	
}
