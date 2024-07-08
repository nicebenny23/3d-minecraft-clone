
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
	inline v3::Vector3 faceoffsetfromcenter(int face) {

		switch (face)
		{
		case 0:
			return v3::Vector3(0,0,1);
		case 1:
			return v3::Vector3(0, 0, -1);
		case 2:
			return  v3::Vector3(1, 0, 0);
		case 3:
			return v3::Vector3(-1, 0, 0);
		case 4:
			return v3::Vector3(0, 1, 0);
		case 5:
			return v3::Vector3(0,-1, 0);
		default:
			Assert("atempetd to acess a nonexistant direction");

			break;
		}
	}
	struct block;
	struct face {
		block* holder;
		bool covered;
		byte tex;
		int facenum;
		float cameradist;
		face();
		face(byte texval, int num, block* owner);

		

		//only set correctly during frame of render for transparent objs
	
	};
		
		
		
		

	
	struct block:gameobject::obj
	{
		bool transparent;
		bool solid;
		byte id;
		v3::Coord pos;
		
		face up;
	    face left;
		face right;
		face down ;
		face front;
		face back ;
		face& operator[](int index) {

			switch (index)
			{
			case 0:
				return front;
			case 1:
				return back;
			case 2:
				return left;
			case 3:
				return right;
			case 4:
				return up;
			case 5:
				return down;
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
	void createfaces(block*blk);
}


	inline void calccameradist(blockname::face* fc) {

		v3::Vector3 center = fc->holder->pos + unitv / 2 + blockname::faceoffsetfromcenter(fc->facenum)/2.001-Vector3(camera::campos) ;
		//todo find out why this has error when distance =0;
	//incrses accuracy
		fc->cameradist =128*magnitude2(center);
		int l = 1;

	}

#endif // !block_H
