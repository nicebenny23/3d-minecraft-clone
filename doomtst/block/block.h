#include "../util/dir.h"
#include "../renderer/renderer.h"
#include <glm/glm.hpp>
#include "../game/gameobject.h"
#include "../game/aabb.h"
#include "../game/camera.h"
#ifndef block_HPP

#define block_HPP

constexpr auto blocksize = 1.f;
enum blocktex {
	treestonetex = 0,
	grasstex = 1,
	stonetex = 2,
	altartex = 3,
	glasstex = 4,
	watertex = 5,
	torchtex = 6,
	torchtoptex = 7,
	crystaloretex = 8,
	craftingtabletop= 9,
	craftingtableside=10,
	crystaltorchtex = 11,
	crystaltorchtoptex = 12,
	mosstex=13,
	ropetex=14,
	lavatex=15,
	obsidiantex=16,
	chestfront=17,
	chestside=18,
	furnaceside=19,
	furnacefront=20,
	ironoretex=21,
	furnacefronton=22,
	furnacesideon=23,
	logtoppng=24,
	ultraaltarpngultrapng=25,
	sandtex=26,
	planktex=27,
};
using namespace v3;

namespace blockname {
	const v3::Vector3 unitscale = unitv*blocksize * 1 / 2.002f;
	enum id
	{
		minecraftair = 0,
		minecrafttreestone = 1,
		minecraftmoss = 2,
		minecraftstone = 3,

		minecraftglass = 4,
		minecraftwater = 5,
		minecraftcrystal = 6,
		minecrafttorch = 7,
		minecraftsand = 8,
		minecraftcraftingtable=9,
		minecraftcrystaltorch=10,
		minecraftrope=11,
		minecraftlava=12,
		minecraftobsidian=13,
		minecraftchest=14,
		minecraftfurnace=15,
		minecraftironore=16,
		minecraftaltar=17,
		minecraftplank=18,
	};
	
	struct blockmesh;
	struct face {
		

		blockmesh* mesh;

		float cameradist;
		byte tex;
		int facenum:4;
		byte light;
	

		bool covercomputed;
		bool covered;
		face() {
			covercomputed = false;
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
			covercomputed = false;
			mesh = owner;
			light = 0;
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
		byte direction;
		byte attachdir;
		Vector3 pos;
		Vector3 scale;
		face faces[6];
		block* blk;
		
		face& operator[](int index);
		void attachindirection();
		void setfaces(int leftface, int rightface, int upface, int downface, int frontface, int backface);
		
	};
	
		
	

	









	struct blockstate {

		bool broken;
		blockstate() {
			broken = false;
		}
	};
	struct blockatt
	{
		bool transparent;
		bool solid;
	};
	struct  block:gameobject::obj
	{

		blockmesh mesh;

		v3::Coord pos;
		byte emitedlight;
		byte lightval;
		byte id;

		blockstate bstate;
	
		face& operator[](int index);
		blockatt attributes;
		
		
		byte mininglevel;
		bool minedfastwithpick;
		Vector3 center() {
			return Vector3( pos)*blocksize + unitv *blocksize/ 2;
	    }
		

		 block(v3::Coord location,int blockid);
		 block();
		 	 void createaabb(bool effector=false);
		
	};

	void initblockmesh(block* blk, Vector3 pos, Vector3 scale);

}
//if you have a\


	

#endif // !block_H
