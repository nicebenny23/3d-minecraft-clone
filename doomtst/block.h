
#include "renderer.h"
#include <glm/glm.hpp>
#include "gameobject.h"
#include "aabb.h"
#ifndef block_HPP
#define block_HPP
using namespace v3;
namespace blockname {

	enum id
	{
		minecraftair = (0),

		minecraftdirt = 1,
		minecraftgrass = 2,
		minecraftstone = (3),
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
			assert("1");

			break;
		}
	}
	struct face {
		
		byte tex;
		face() {
			tex = -1;
		}
		face(byte texval) {
			tex = texval;
		}
	};
	struct facetex
	{
		
		face up;
		face left;
		face right;
		face down;
		face front;
		face back;
		int texatface(int index) {
			return (*this)[index].tex;

		}
		face operator[](int index) {
			
			switch (index)
			{
			case 0:
				return back;
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
				assert("1");
				
				break;
			}
		}
		
		facetex(int id) {

			switch (id)
			{
			case minecraftdirt:
				up = face(0);
				left =face(0);
				right =face(0);
				down =face(0);
				front =face(0);
				back =face(0);
				break;
			case minecraftgrass:
				up =face(1);
				left =face(0);
				right =face(0);
				down =face(0);
				front =face(0);
				back =face(0);
				break;
			case minecraftstone:
				up =face(3);
				left =face(2);
				right =face(2);
				down =face(3);
				front =face(2);
				back =face(2);
				break;
			case minecraftglass:
				back =face(4);
				break;
			case minecraftwater:
				back = face(5);
				break;
			case minecraftair:
				
					break;
			}
		}

		facetex() = default;
	};
	struct block:gameobject::obj
	{

		
		byte id;
		v3::Coord pos;
		facetex tex;
	
 void render();

		 block(v3::Coord placment,int setid);
		 block();
		 void createaabb();
	};
	void setair(blockname::block* blk);
	void giveblocktraits(blockname::block* nullblock);
}


#endif // !block_H
