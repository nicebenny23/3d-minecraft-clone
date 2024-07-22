#include "model.h"
#include "../game/objecthelper.h"
meshname::mesh& model::operator[](int index)
{
	return *meshlist[index];
}
void model::draw()
{
}
void model::add(const char* meshname,const char * meshtexname)
{
		texture meshtex = texture(meshtexname,png);
	meshlist.append(meshname::loadmesh(meshname, meshtex, zerov));
	meshlist[meshlist.length - 1]->modeltranform = &objutil::toent(owner).transform;
}
model::model()
{

	meshlist = array<meshname::mesh*>();
}

void model::renderupdate()
{
	for (int i = 0; i < meshlist.length; i++)
	{
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
