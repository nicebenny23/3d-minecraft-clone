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
{
	color=unitv;
	transform = Transform();
}

void ModelMeshName::ModelMesh::create_handle(const char* locaion,const char* name)
{
handle=CtxName::ctx.Ren->gen_renderable(); 
handle.set_layout(vertice::vertex().push<float, 3>().push<float, 2>());
handle.set_material("Model");
handle.set_uniform(uniforms::uniform(CtxName::ctx.Ren->Textures.LoadTexture(locaion, name), "tex"));
}

Point3 ModelMesh::nthvertex(size_t i)
{
	size_t ind = vertexindices[i] - 1;


	return vertices[ind];
}

v2::Vec2 ModelMesh::nthtex(size_t i)
{
	return texcoords[texindices[i] - 1];
}

ModelMesh ModelMeshName::loadmesh(const char* location,const char* name, Vec3 position)
{
	
	ModelMesh newmesh = ModelMesh();
	safefile meshfile = safefile(name, fileread);
	newmesh.pos = position;
	while (true)
	{
		char* header = new char[128];

		int test = fscanf(meshfile.fp, "%s", header);
		if (test == EOF)
		{
			break;
		}
		if (strcmp(header, "v") == 0) {
			float c1 = 0, c2 = 0, c3 = 0;
			meshfile.fscanf(3, "%f %f %f\n", &c1, &c2, &c3);
			newmesh.vertices.push(Point3(c1,c2,c3));
		}
		if (strcmp(header, "vt") == 0) {
			v2::Vec2 texcoord;
			float c1 = 0, c2 = 0;
			meshfile.fscanf(2,"%f %f\n", &c1, &c2);
		
			newmesh.texcoords.push(v2::Vec2(c1,c2));
		}
		if (strcmp(header, "f") == 0) {

			unsigned int* vertexIndex, * uvIndex;
			vertexIndex = new unsigned int[3];
			uvIndex = new unsigned int[3];
			 meshfile.fscanf(6,"%u/%u %u/%u %u/%u\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]);
			newmesh.vertexindices.push(vertexIndex[0]);
			newmesh.vertexindices.push(vertexIndex[1]);
			newmesh.vertexindices.push(vertexIndex[2]);
			newmesh.texindices.push(uvIndex[0]);
			newmesh.texindices.push(uvIndex[1]);
			newmesh.texindices.push(uvIndex[2]);
			delete[] uvIndex;
			delete[] vertexIndex;
		}

		delete[] header;
	}
	newmesh.create_handle(location,name);

	return newmesh;
}

void ModelMeshName::rendermesh(ModelMesh& torender)
{
//	CtxName::ctx.Ren->Bind_Texture(torender->tex);
	
	renderer::MeshData data=torender.handle.create_mesh(renderer::indice_mode::generate_indices);
	v3::Point3 pos = torender.transform.position;
	
	torender.setmodeluniform();
	torender.handle.set_uniform(uniforms::uniform(torender.color,"col"));
	for (int i = 0; i < torender.vertexindices.length(); i++)
	{
		Point3 vertex = torender.nthvertex(i);
		v2::Vec2 Texture2D = torender.nthtex(i);
		Texture2D.y= 1 - Texture2D.y;
		data.add_point(vertex, Texture2D);
	

	}
	

	torender.handle.fill(std::move(data));	//enable position
		CtxName::ctx.Ren->pop();
		torender.handle.render();
}
