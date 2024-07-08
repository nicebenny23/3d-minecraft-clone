/*#include "gameobject.h"
#include "vector3.h"

using namespace v3;
#ifndef blkmesh_HPP
#define blkmesh_HPP
//inline void calccameradist(face* fc) {



	//incrses accuracy


//}
struct face {
	blockmesh* holder;
	bool covered;
	byte tex;
	int facenum;
	float cameradist;
	
	face(byte texval, int num, blockmesh* owner);
		Vector3 center;
	void calccameradist();

};
struct blockmesh :gameobject::component
{



	Vector3 pos;
	Vector3 scale;
	face up;
	face left;
	face right;
	face down;
	face front;
	face back;
	face& operator[](int index);
	void createfaces();
	
	void createaabb() {


	}
};
#endif // !blkmesh_HPP
*/