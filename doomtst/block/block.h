#include "../util/dir.h"
#include "../renderer/renderer.h"
#include <glm/glm.hpp>
#include "../game/gameobject.h"
#include "../game/aabb.h"
#include "../game/camera.h"


#ifndef block_HPP
#define block_HPP
using namespace v3;
namespace blockname {
	enum id
	{
		minecraftair = 0,
		minecraftdirt = 1,
		minecraftgrass = 2,
		minecraftstone = 3,
		minecraftglass = 4,
		minecraftwater = 5,
	};

	struct blockmesh;
	struct face {
		blockmesh* mesh;
		bool covered;
		byte tex;
		int facenum;
		float cameradist;


		face() {
			mesh= nullptr;
			covered = false;
			facenum = -1;
			cameradist = -1;
		}
		face(byte texval, int num, blockmesh* owner) {
			tex = texval;
			facenum = num;
			mesh = owner;
			calccameradist();
		}
		Vector3 center();
		void calccameradist();

	};
	struct block;
	struct blockmesh
	{

		blockmesh() {

			pos = zerov;
			scale = zerov;
			blk = nullptr;
		}
		blockmesh(block* parent) {

			pos = zerov;
			scale = zerov;
			blk = parent;
		}

		Vector3 pos;
		Vector3 scale;
		face up;
		face left;
		face right;
		face down;
		face front;
		face back;
		block* blk;
		face& operator[](int index);

		void setfaces(int leftface, int rightface, int upface, int downface, int frontface, int backface);
		
	};
	
		
	

	











	struct block:gameobject::obj
	{
		bool transparent;
		bool solid;
		byte id;
		v3::Coord pos;
		blockmesh* mesh;
		
		void createfaces();
		
		Vector3 center() {
			return pos + unitv / 2.001;
	}
		


		 block(v3::Coord placment,int blockid);
		 block();
		 void createaabb();
	};
	void setair(blockname::block* blk);
	void giveblocktraits(blockname::block* nullblock);

	

}
//if you have a\


	

#endif // !block_H
