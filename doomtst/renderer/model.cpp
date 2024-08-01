#include "model.h"
#include "../game/objecthelper.h"
meshname::mesh& model::operator[](int index)
{
	return *meshlist[index];
}
void model::draw()
{
}
void model::add(const char* meshname,const char * meshtexname,Vector3 pos)
{
		texture meshtex = texture(meshtexname,png);
	meshlist.append(meshname::loadmesh(meshname, meshtex, zerov));
meshlist[meshlist.length - 1]->transform.position = pos;
}
model::model(meshconnecttype connectmethod )
{
	connectiontype = connectmethod;
	meshlist = array<meshname::mesh*>();
}

void model::renderupdate()
{
	renderer::changerendertype(renderer::rendermodel);
	glm::mat4* model =new glm::mat4(transformtomat(objutil::toent(owner).transform));
	for (int i = 0; i < meshlist.length; i++)
	{
		meshlist[i]->modelmatrix =model;
		meshname::rendermesh(meshlist[i]);
	}
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
	if (owner->type!=gameobject::block)
	{



	}
}
