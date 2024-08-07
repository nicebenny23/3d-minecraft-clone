#include "../util/dynamicarray.h"
#include "../util/fileloader.h"
#include "../util/vector2.h"
#include "../util/vector3.h"
#include "texture.h"
#include "vertexobject.h"
#include "vertexobject.h"
#include "../game/transform.h"


using namespace dynamicarray;
using namespace v3;
using namespace vobj;
#ifndef  mesh_H
#define  mesh_H
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
namespace meshname {
	
	struct mesh
	{
		float yaw;
		float pitch;
		v3::Vector3 pos;
		void setmodeluniform();
		
		void setmodeluniform(glm::mat4 model);

		texture tex;
		mesh();
	
		Transform transform;
		array<Vector3> vertices;
		array<v2::Vector2> texcoords;
		array<unsigned int> vertexindices;
		array<unsigned int> texindices;
		void destroy() {

			meshvbuf.destroy();
			meshvao.destroy();
			tex.destroy();
			vertexindices.destroy();
			texcoords.destroy();
			texindices.destroy();
			vertices.destroy();
		}
		Vector3 nthvertex(int i);
		v2::Vector2 nthtex(int i);
		vobj::vao meshvao;
		vobj::vbuf meshvbuf;
		void setvobjs() {

			meshvbuf.generate(GL_ARRAY_BUFFER);
			meshvao.generate();
		}

		glm::mat4* modelmatrix;

	};	mesh* loadmesh(const char* name, texture TEX, Vector3 position);
	void rendermesh(mesh* torender);

}
#endif // ! mesh_H
