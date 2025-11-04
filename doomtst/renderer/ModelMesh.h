#pragma once
#include "../util/dynamicarray.h"
#include "../util/fileloader.h"
#include "../util/vector2.h"
#include "../util/vector3.h"

 #include "../game/GameContext.h"
#include "../game/transform.h"
#include "renderer.h"
//fix
using namespace stn;
using namespace v3;
using namespace buffer_object;
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS

#define _CRT_SECURE_NO_DEPRECATE
namespace ModelMeshName {
	
	struct ModelMesh
	{

		float yaw;
		float pitch;
		v3::Vec3 pos;
		void setmodeluniform();
		stn::Id tex;
		Vec3 color;
		ModelMesh();
		void create_handle(const char* location, const char* name);
		Transform transform;
		array<Vec3> vertices;
		array<unsigned int> vertexindices;
		array<v2::Vec2> texcoords;
		array<unsigned int> texindices;
		void destroy() {
			handle.destroy();
		}
		Vec3 nthvertex(size_t i);
		v2::Vec2 nthtex(size_t i);
		renderer::RenderableHandle handle;
		

		glm::mat4* modelmatrix;


	};	ModelMesh loadmesh(const char* location, const char* name, Vec3 position);
	void rendermesh(ModelMesh& torender);

}
