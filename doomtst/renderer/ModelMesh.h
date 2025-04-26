#include "../util/dynamicarray.h"
#include "../util/fileloader.h"
#include "../util/vector2.h"
#include "../util/vector3.h"
#include "texture.h"
#include "vertexobject.h"
#include "Vao.h"
#include "../game/transform.h"
#include "renderer.h"
#pragma once
using namespace dynamicarray;
using namespace v3;
using namespace vobj;

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
namespace ModelMeshName {
	
	struct ModelMesh
	{
		float yaw;
		float pitch;
		v3::Vector3 pos;
		void setmodeluniform();
		
		void setmodeluniform(glm::mat4 model);

		Texture2D* tex;
		ModelMesh();
	
		Transform transform;
		array<Vector3> vertices;
		array<v2::Vector2> texcoords;
		array<unsigned int> vertexindices;
		array<unsigned int> texindices;
		void destroy() {
			renderer::Ren.Destroy(&mesh);
			vertexindices.destroy();
			texcoords.destroy();
			texindices.destroy();
			vertices.destroy();
		}
		Vector3 nthvertex(int i);
		v2::Vector2 nthtex(int i);
		Mesh mesh;
		void setvobjs() {
		renderer::Ren.Gen<false>(&mesh);

			mesh.AddAttribute<float,2>().AddAttribute<float, 3>();
		}

		glm::mat4* modelmatrix;

	};	ModelMesh* loadmesh(const char* name, Texture2D* TEX, Vector3 position);
	void rendermesh(ModelMesh* torender);

}
