#pragma once
#include "../util/dynamicarray.h"
#include "../util/fileloader.h"
#include "../util/vector2.h"
#include "../util/vector3.h"
#include "texture.h"
#include "vertexobject.h"
#include "../game/GameContext.h"
#include "Vao.h"
#include "../game/transform.h"
#include "renderer.h"
//fix
using namespace stn;
using namespace v3;
using namespace buffer_object;

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
namespace ModelMeshName {
	
	struct ModelMesh
	{
		float yaw;
		float pitch;
		v3::Vec3 pos;
		void setmodeluniform();
		
		void setmodeluniform(glm::mat4 model);

		Texture2D* tex;
		ModelMesh();
	
		Transform transform;
		array<Vec3> vertices;
		array<v2::Vec2> texcoords;
		array<unsigned int> vertexindices;
		array<unsigned int> texindices;
		void destroy() {
			CtxName::ctx.Ren->Destroy(&mesh);
			vertexindices.destroy();
			texcoords.destroy();
			texindices.destroy();
			vertices.destroy();
		}
		Vec3 nthvertex(int i);
		v2::Vec2 nthtex(int i);
		Mesh mesh;
		void setvobjs() {
		CtxName::ctx.Ren->Gen(&mesh);

			mesh.Voa.attributes.push<float,2>().push<float, 3>();
		}

		glm::mat4* modelmatrix;

	};	ModelMesh* loadmesh(const char* name, Texture2D* TEX, Vec3 position);
	void rendermesh(ModelMesh* torender);

}
