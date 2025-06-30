#pragma once
#include "ModelMesh.h"
#include "../game/camera.h"

using namespace ModelMeshName;
void ModelMesh::setmodeluniform()
{	
	glm::mat4 trans =*modelmatrix;
	
	glm::mat4 trans2 = (transform.ToMatrix());
	
	trans *= trans2;
	renderer::Ren.CurrentShader()->setMat4(trans, "model");
}
void ModelMesh::setmodeluniform(glm::mat4 model)
{

	renderer::Ren.CurrentShader()->setMat4(model, "model");
}
ModelMesh::ModelMesh()
{	transform = Transform();
}

Vector3 ModelMesh::nthvertex(int i)
{

	return vertices[vertexindices[i] - 1];
}

v2::Vector2 ModelMesh::nthtex(int i)
{
	return texcoords[texindices[i] - 1];
}

ModelMesh* ModelMeshName::loadmesh(const char* name, Texture2D* TEX, Vector3 position)
{

	ModelMesh* newmesh = new ModelMesh();
	safefile meshfile = safefile(name, fileread);
	newmesh->pos = position;
	newmesh->tex = TEX;
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
			Vector3 vertex;
			meshfile.fscanf(3, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			newmesh->vertices.push(vertex);
		}
		if (strcmp(header, "vt") == 0) {
			v2::Vector2 texcoord;
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
	renderer::Ren.Textures.Apply(torender->tex);
	
	array<float> databuf;

	torender->setmodeluniform();

	for (int i = 0; i < torender->vertexindices.length; i++)
	{
		Vector3 vertex = torender->nthvertex(i);
		v2::Vector2 Texture2D = torender->nthtex(i);

		databuf.push(vertex.x);
		databuf.push(vertex.y);
		databuf.push(vertex.z);
		databuf.push(Texture2D.x);
		databuf.push(1-Texture2D.y);

	}
	renderer::Ren.Render(&torender->mesh, databuf);


	//enable position

	glDrawArrays(GL_TRIANGLES, 0, databuf.length / 5);
	databuf.destroy();
}
