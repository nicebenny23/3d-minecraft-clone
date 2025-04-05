#include "../util/dir.h"
#include "../renderer/renderer.h"
#include <glm/glm.hpp>
#include "../game/gameobject.h"
#include "../game/aabb.h"
#include "../game/camera.h"

#include "../util/geometry.h"
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
#define  unitscale   unitv * 1 / 2.002f
namespace blockname {
	const v3::Vector3 blockscale = unitscale * blocksize;
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
		byte facenum;
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
	
		void create(byte texval, int num, blockmesh* owner) {
			covercomputed = false;
			tex = texval;
			facenum = num;
			mesh = owner;
		}
		Vector3 center();
		void calccameradist();

	};
	struct block;


	struct blockmesh
	{
		byte direction;
		byte attachdir;
		blockmesh() {

			box.center = zerov;
			box.scale = zerov;
			blk = nullptr;
		}	
	
		blockmesh(block* parent,  Vector3 blkscale);
		geometry::Box box;
		block* blk;
	
		
		face faces[6];
		
		
		
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
		void create(v3::Coord location, int blockid, byte blkattachface, byte blkdirection);

		block() {};
		 	 void createdefaultaabb(bool effector=false);
		
	};


}
//if you have a\


	

#endif // !block_H
