#include "util/dynamicarray.h"
#include "util/fileloader.h"
#include "util/vector2.h"
#include "util/vector3.h"
#include "renderer/texture.h"
#include "renderer/vertexobject.h"
#include "renderer/vertexobject.h"
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
using namespace dynamicarray;
using namespace v3;
using namespace vobj;
#ifndef  mesh_H
#define  model_H
namespace meshname {

	struct mesh
	{
		v3::Vector3 rotation;
		v3::Vector3 pos;
		void setmodeluniform();
		texture tex;
		mesh();
		array<Vector3> vertices;
		array<v2::Vector2> texcoords;
		array<unsigned int> vertexindices;
		array<unsigned int> texindices;
		Vector3 nthvertex(int i);
		v2::Vector2 nthtex(int i);
		vobj::vao meshvao;
		vobj::vbuf meshvbuf;
		void setvobjs() {

			meshvbuf.generate(GL_ARRAY_BUFFER);
			meshvao.generate();
		}
	};

	mesh* loadmesh(const char* name,texture TEX,Vector3 position);
	void rendermesh(mesh* torender);
}
#endif // ! mesh_H
