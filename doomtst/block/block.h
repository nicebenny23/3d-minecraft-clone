#include "../math/dir.h"
#include "../renderer/renderer.h"
#include <glm/glm.hpp>
#include "../game/camera.h"
#include "../game/ecs/game_object.h"
#include "../math/geometry.h"
#include "../math/geometry.h"
#include <rpcndr.h>
#include "../game/ecs/component.h"
#include "../math/Scale3.h"
#include "../util/Option.h"
#include "../math/vector3.h"
#pragma once 
constexpr double blocksize = 1.f;
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
constexpr float unitaxis = 1.0f / 2.00005f;
constexpr v3::Scale3  unitscale = unit_scale*unitaxis;
namespace blockname {
	const double block_axis_scale = unitaxis * blocksize;
	const v3::Scale3 blockscale =  v3::Scale3(block_axis_scale);

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
	enum class cover_state :char {
		Covered=0,
		Uncovered=1,
		Uncomputed=2

	};
	struct face {
		

		blockmesh* mesh;

		byte tex;
		Dir::Dir3d facenum;
		std::uint8_t light;
		bool uncomputed() const {
			return cover == cover_state::Uncomputed;
		}
		bool uncovered() const {
			return cover == cover_state::Uncovered;
		}
		bool covered() const {
			return cover == cover_state::Covered;
		}
		cover_state cover;
		face() {
			cover = cover_state::Uncomputed;
			mesh= nullptr;
			facenum = Dir::None3d;
			tex = 0;
			light = 0;
		}
	
		void create(byte texval,  char num, blockmesh* owner) {
			cover= cover_state::Uncomputed;
			tex = texval;
			facenum =Dir::Dir3d(num);
			mesh = owner;
		}
		Point3 center();

	};
	struct block;


	struct blockmesh
	{
		Dir::Dir2d direction;
		Dir::Dir3d attachdir;
		blockmesh():box(Point3(0,0,0), v3::unit_scale) {
			blk = nullptr;
		}	
		Point3 center() const {
			return box.center;
		}

		Scale3 scale() const {
			return box.scale;
		}
		geometry::Box bounds() const {
			return box;
		}
		bool invisible() const;
		blockmesh(block* parent,  Scale3 blkscale);
		geometry::Box box;
		block* blk;
		face faces[6];
		face& operator[](size_t index);
		const face& operator[](size_t index) const;
		void attachindirection();
		void setfaces(int leftface, int rightface, int upface, int downface, int frontface, int backface);
		
	};
	
		

	struct blockatt
	{
		bool transparent;
		bool solid;
		blockatt() {
			solid = true;
			transparent = false;
		}
	};
	struct  block: ecs::component
	{

		blockmesh mesh;

		v3::Coord pos;
		size_t emitedlight;
		stn::Option<size_t> lightval;
		char id;

		
	
		face& operator[](size_t index);
		blockatt attributes;
		
	
		char mininglevel;
		bool minedfastwithpick;
		Point3 center() const{
			return mesh.box.center;
		}

		Scale3 scale() const{
			return mesh.box.scale;
		}
		geometry::Box bounds() const {
			return mesh.box;
		}

		void create(v3::Coord location, int blockid, Dir::Dir3d blkattachface, Dir::Dir2d blkdirection);
		
		block() = default;
		 	 void createdefaultaabb(bool effector=false);
		
	};

}
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<blockname::block> = ComponentInfo{ .updates = false };
}
//if you have a\


	

 // !block_H
