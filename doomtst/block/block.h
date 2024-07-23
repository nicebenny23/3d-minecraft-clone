#include "../util/dir.h"
#include "../renderer/renderer.h"
#include <glm/glm.hpp>
#include "../game/gameobject.h"
#include "../game/aabb.h"
#include "../game/camera.h"

#ifndef block_HPP
#define block_HPP
enum blocktex {
	dirttex = 0,
	grasstex = 1,
	stonetex = 2,
	stonetex1 = 3,
	glasstex = 4,
	watertex = 5,
	torchtex = 6,
	torchtoptex = 7,
	crystaloretex = 8,
	
};
using namespace v3;

namespace blockname {
	const v3::Vector3 unitscale = unitv * 1 / 2.002;
	enum id
	{
		minecraftair = 0,
		minecraftdirt = 1,
		minecraftgrass = 2,
		minecraftstone = 3,

		minecraftglass = 4,
		minecraftwater = 5,
		minecraftcrystal = 6,
		minecrafttorch = 7,
		minecraftwood = 8,
		minecraftcraftingtable=9,
	};
	struct blockmesh;
	struct face {
		
		blockmesh* mesh;
		bool covered;
		byte tex;
		int facenum;
		float cameradist;
		int light;

		face() {
			mesh= nullptr;
			covered = false;
			facenum = -1;
			cameradist = -1;
			tex = 0;
			light = 0;
		}
		face(byte texval, int num, blockmesh* owner) {
			tex = texval;
			facenum = num;
			mesh = owner;
			light = 0;
			calccameradist();
		}
		Vector3 center();
		void calccameradist();

	};
	struct block;


	enum directionmode {
		attachdir = 0,
		orientdir=1,
		ignoredir=2,
	};
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
		int direction;
		int attachdir;
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
		void attachindirection();
		void setfaces(int leftface, int rightface, int upface, int downface, int frontface, int backface);
		
	};
	
		
	

	











	struct block:gameobject::obj
	{
		face& operator[](int index);
		bool transparent;
		int emitedlight;
		int lightval;
		bool solid;
		byte id;
		v3::Coord pos;
		blockmesh mesh;
		float mininglevel;
	
		Vector3 center() {
			return pos + unitv / 2;
	    }
		

		 block(v3::Coord location,int blockid);
		 block();
		 void initasgameobj();
		 void createaabb();
		
	};

	void initblockmesh(block* blk, Vector3 pos, Vector3 scale);

}
//if you have a\


	

#endif // !block_H
