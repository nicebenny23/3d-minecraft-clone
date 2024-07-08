#include "dir.h"
#include "renderer.h"
#include <glm/glm.hpp>
#include "gameobject.h"
#include "aabb.h"
#include "camera.h"
#ifndef block_HPP
#define block_HPP
using namespace v3;
namespace blockname {

	enum id
	{
		minecraftair =   0,
		minecraftdirt =  1,
		minecraftgrass = 2,
		minecraftstone = 3,
		minecraftglass = 4,
		minecraftwater = 5,
	};
	
	struct block;
	
		
		
	struct face {
		block* holder;
		bool covered;
		byte tex;
		int facenum;
		float cameradist;
		face() {
			holder = nullptr;
			covered = false;
			facenum = -1;
			cameradist = -1;
		}
		face(byte texval, int num, block* owner) {
			tex = texval;
			facenum = num;
			holder = owner;
			cameradist = -1;
		}
			Vector3 center;
		void calccameradist();

	};

	
	struct block:gameobject::obj
	{
		bool transparent;
		bool solid;
		byte id;
		v3::Coord pos;

		Vector3 blkoffset;
		Vector3 scale;
		Vector3 center() {
			return pos + unitv / 2;

		}
		face up;
		face left;
		face right;
		face down;
		face front;
		face back;
		face& operator[](int index);
		void createfaces();
		Vector3 center() {
			return pos + unitv / 2+blkoffset;

		}
	
		face& operator[](int index)
		{


			switch (index)
			{
			case 0:
				return left;
			case 1:
				return right;

			case 2:
				return up;
			case 3:
				return down;
			case 4:
				return front;
			case 5:
				return back;

			default:
				Assert("index too high/low for faces");

				break;

			}

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
