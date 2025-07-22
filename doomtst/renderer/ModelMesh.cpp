#pragma once
#include "ModelMesh.h"
#include "../game/camera.h"
#include "../game/GameContext.h"
using namespace ModelMeshName;
void ModelMesh::setmodeluniform()
{	
	glm::mat4 trans =*modelmatrix;
	
	glm::mat4 trans2 = (transform.ToMatrix());
	
	trans *= trans2;
	handle.set_uniform(uniforms::uniform(trans, "model"));
}

ModelMesh::ModelMesh()
{	transform = Transform();
}

void ModelMeshName::ModelMesh::create_handle()
{
handle=CtxName::ctx.Ren->gen_renderable(); 
handle.set_layout(vertice::vertex().push<float, 3>().push<float, 3>());
}

Vec3 ModelMesh::nthvertex(int i)
{

	return vertices[vertexindices[i] - 1];
}

v2::Vec2 ModelMesh::nthtex(int i)
{
	return texcoords[texindices[i] - 1];
}

ModelMesh* ModelMeshName::loadmesh(const char* name, Ids::Id tex_handle, Vec3 position)
{

	ModelMesh* newmesh = new ModelMesh();
	safefile meshfile = safefile(name, fileread);
	newmesh->pos = position;
	newmesh->tex = tex_handle;
	newmesh->setvobjs();
	while (true)
	{
		char* header = new char[128];

		int test = fscanf(meshfile.fp, "%s", header);
		if (test == EOF)
		{
			break;
		}
		if (strcmp(header, "v") == 0) {
			Vec3 vertex;
			meshfile.fscanf(3, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			newmesh->vertices.push(vertex);
		}
		if (strcmp(header, "vt") == 0) {
			v2::Vec2 texcoord;
			meshfile.fscanf(2,"%f %f\n", &texcoord.x, &texcoord.y);
		
			newmesh->texcoords.push(texcoord);
		}
		if (strcmp(header, "f") == 0) {

			unsigned int* vertexIndex, * uvIndex;
			vertexIndex = new unsigned int[3];
			uvIndex = new unsigned int[3];
			 meshfile.fscanf(6,"%u/%u %u/%u %u/%u\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]);
			newmesh->vertexindices.push(vertexIndex[0]);
			newmesh->vertexindices.push(vertexIndex[1]);
			newmesh->vertexindices.push(vertexIndex[2]);
			newmesh->texindices.push(uvIndex[0]);
			newmesh->texindices.push(uvIndex[1]);
			newmesh->texindices.push(uvIndex[2]);
			delete[] uvIndex;
			delete[] vertexIndex;
		}

		delete[] header;
	}

	return newmesh;
}

void ModelMeshName::rendermesh(ModelMesh* torender)
{
//	CtxName::ctx.Ren->Bind_Texture(torender->tex);
	
	renderer::MeshData data();

	torender->setmodeluniform();

	for (int i = 0; i < torender->vertexindices.length; i++)
	{
		Vec3 vertex = torender->nthvertex(i);
		v2::Vec2 Texture2D = torender->nthtex(i);
		Texture2D.y= 1 - Texture2D.y;
		data.add_point(vertex, Texture2D);
	

	}
	if (!torender)
	{

		torender->create_handle();

	}

torender->handle.fill(	//enable position

	glDrawArrays(GL_TRIANGLES, 0, databuf.length / 5);
	databuf.destroy();
}
