#include "mesh.h"
#include "renderer.h"
#include "vertexobject.h"
#include "../game/camera.h"

using namespace meshname;
void meshname::mesh::setmodeluniform()
{	
	glm::mat4 trans =*modelmatrix;
	
	glm::mat4 trans2 = transformtomat(transform);
	
	trans *= trans2;
	renderer::shaderlist[renderer::modelshader].setmatval(trans, "model");
}
void meshname::mesh::setmodeluniform(glm::mat4 model)
{

	renderer::shaderlist[renderer::modelshader].setmatval(model, "model");
}
meshname::mesh::mesh()
{	transform = Transform();
}

Vector3 meshname::mesh::nthvertex(int i)
{

	return vertices[vertexindices[i] - 1];
}

v2::Vector2 meshname::mesh::nthtex(int i)
{
	return texcoords[texindices[i] - 1];
}

meshname::mesh* meshname::loadmesh(const char* name, texture TEX, Vector3 position)
{

	mesh* newmesh = new mesh();
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
			fscanf(meshfile.fp, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			newmesh->vertices.append(vertex);
		}
		if (strcmp(header, "vt") == 0) {
			v2::Vector2 texcoord;
			fscanf(meshfile.fp, "%f %f\n", &texcoord.x, &texcoord.y);
			newmesh->texcoords.append(texcoord);
		}
		if (strcmp(header, "f") == 0) {

			unsigned int* vertexIndex, * uvIndex;
			vertexIndex = new unsigned int[3];
			uvIndex = new unsigned int[3];
			int matches = fscanf(meshfile.fp, "%u/%u %u/%u %u/%u\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]);
			if (matches != 6)
			{
				Assert("Mesh not triangulated correctly ");
			}
			newmesh->vertexindices.append(vertexIndex[0]);
			newmesh->vertexindices.append(vertexIndex[1]);
			newmesh->vertexindices.append(vertexIndex[2]);
			newmesh->texindices.append(uvIndex[0]);
			newmesh->texindices.append(uvIndex[1]);
			newmesh->texindices.append(uvIndex[2]);
			delete[] uvIndex;
			delete[] vertexIndex;
		}

		delete[] header;
	}

	return newmesh;
}

void meshname::rendermesh(mesh* torender)
{

	torender->tex.apply();
	array<float> databuf;

	torender->setmodeluniform();

	for (int i = 0; i < torender->vertexindices.length; i++)
	{
		Vector3 vertex = torender->nthvertex(i);
		v2::Vector2 texture = torender->nthtex(i);

		databuf.append(vertex.x);
		databuf.append(vertex.y);
		databuf.append(vertex.z);
		databuf.append(texture.x);
		databuf.append(1-texture.y);

	}
	torender->meshvbuf.bind();

	torender->meshvbuf.fillbuffer<float>(databuf);



	//uv
	torender->meshvao.bind();
	torender->meshvao.set_attr(0, 3, GL_FLOAT, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	//texture coords,inclusing the texture in the array 
	torender->meshvao.set_attr(1, 2, GL_FLOAT, 5 * sizeof(float), 3 * sizeof(float));
	glEnableVertexAttribArray(1);


	//enable position

	glDrawArrays(GL_TRIANGLES, 0, databuf.length / 5);
	databuf.destroy();
}
