#include "model.h"
#include "../game/objecthelper.h"
ModelMeshName::ModelMesh& model::operator[](int index)
{
	return *meshlist[index];
}
void model::draw()
{
}
void model::add(const char* meshname,const char * meshtexname,Vector3 pos)
{
		Texture2D* meshtex = renderer::Ren.Textures.Get2dTex(meshtexname,meshtexname);
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
	renderer::changerendertype(renderer::rendermodel);
	glm::mat4* model =new glm::mat4((objutil::toent(owner).transform.ToMatrix()));
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
	if (owner->type!=gameobject::block)
	{



	}
}

void model::setmodelshader()
{
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	renderer::Ren.Shaders.Bind("ModelShader");
	renderer::setrenderingmatrixes();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	
}
