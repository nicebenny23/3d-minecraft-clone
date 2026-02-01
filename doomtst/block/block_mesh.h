#pragma once
#include "../math/vector3.h"
#include "../math/dir.h"
#include "../math/geometry.h"
#include "../util/cached.h"
namespace blocks {

	enum  block_id {
		minecraftair = 0,
		minecrafttreestone = 1,
		minecraftmoss = 2,
		minecraftstone = 3,

		minecraftglass = 4,
		minecraftwater = 5,
		minecraftcrystal = 6,
		minecrafttorch = 7,
		minecraftsand = 8,
		minecraftcraftingtable = 9,
		minecraftcrystaltorch = 10,
		minecraftrope = 11,
		minecraftlava = 12,
		minecraftobsidian = 13,
		minecraftchest = 14,
		minecraftfurnace = 15,
		minecraftironore = 16,
		minecraftaltar = 17,
		minecraftplank = 18,
	};
	enum block_textures {
		treestonetex = 0,
		grasstex = 1,
		stonetex = 2,
		altartex = 3,
		glasstex = 4,
		watertex = 5,
		torchtex = 6,
		torchtoptex = 7,
		crystaloretex = 8,
		craftingtabletop = 9,
		craftingtableside = 10,
		crystaltorchtex = 11,
		crystaltorchtoptex = 12,
		mosstex = 13,
		ropetex = 14,
		lavatex = 15,
		obsidiantex = 16,
		chestfront = 17,
		chestside = 18,
		furnaceside = 19,
		furnacefront = 20,
		ironoretex = 21,
		furnacefronton = 22,
		furnacesideon = 23,
		logtoppng = 24,
		ultraaltarpngultrapng = 25,
		sandtex = 26,
		planktex = 27,
	};
	struct blockmesh;
	enum class cover_state :char {
		Covered = 0,
		Uncovered = 1,
		Uncomputed = 2

	};
	struct face {


		block_textures tex;
		Dir::Dir3d facenum;
		bool uncomputed() const {
			return cover == cover_state::Uncomputed;
		}
		bool uncovered() const {
			return cover == cover_state::Uncovered;
		}
		bool covered() const {
			return cover == cover_state::Covered;
		}
		face():cover(cover_state::Uncomputed),light(0){
			
			facenum = Dir::None3d;
		}
		blockmesh* mesh;
		void create(byte texval, Dir::Dir3d num, blockmesh* owner) {
			facenum = Dir::Dir3d(num);
			mesh = owner;
			tex = block_textures(texval);
		}
		inline Point3 center() const;
		size_t get_light() const{
			return light;
		}

		void set_cover(cover_state new_state);
		void set_light(size_t value);
	private:
		cover_state cover;
		std::uint8_t light;
	};

	struct blockmesh {
		Dir::Dir2d direction;
		Dir::Dir3d attachdir;
		v3::Point3 center() const {
			return box.center;
		}

		Scale3 scale() const {
			return box.scale;
		}
		geo::Box bounds() const {
			return box;
		}
		bool is_transparent() const {
			return transparent;
		}
		//fix this is not supposed to work
		bool invisible() const {
			return box.scale.x==0;
		}
		blockmesh(dirty_flag& d_flag,v3::Coord position, Scale3 blkscale) :
			flag(d_flag), box(position + unitv / 2, blkscale), mesh_location(position) {
			direction = Dir::up2d;
			attachdir = Dir::front3d;
			box.scale = blkscale;
		}
		
		geo::Box box;
		block_id render_id;
		Coord mesh_location;
		face faces[6];
		face& operator[](size_t index) {
			if (6 <= index) {
				stn::throw_logic_error("invalid face index {}", index);
			}
			return faces[index];
		}

		const face& operator[](size_t index) const {
			if (6 <= index) {
				stn::throw_logic_error("invalid face index {}", index);
			}
			return faces[index];
		}


		void attachindirection() {
			Point3 maxpos = center() + (attachdir.to_vec() * box.scale);
			Point3 blkpos = attachdir.to_vec() * 1 / 2 + center();
			box.center += blkpos - maxpos;
		}


		void set_face_textures(block_textures leftface, block_textures rightface, block_textures upface, block_textures downface, block_textures frontface, block_textures backface) {

			faces[2].create(upface, Dir::Ind3d::Up, this);
			faces[3].create(downface, Dir::Ind3d::Down, this);

			switch (Dir::Ind2d(direction.ind())) {

			case Dir::Ind2d::Left://west
			faces[0].create(frontface, Dir::Ind3d::Right, this);
			faces[1].create(backface, Dir::Ind3d::Left, this);
			faces[4].create(rightface, Dir::Ind3d::Front, this);
			faces[5].create(leftface, Dir::Ind3d::Back, this);
			break;
			case  Dir::Ind2d::Right://east
			faces[0].create(backface, Dir::Ind3d::Right, this);
			faces[1].create(frontface, Dir::Ind3d::Left, this);
			faces[4].create(leftface, Dir::Ind3d::Front, this);
			faces[5].create(rightface, Dir::Ind3d::Back, this);
			break;
			case Dir::Ind2d::Up://block faces fowards
			faces[0].create(leftface, Dir::Ind3d::Right, this);
			faces[1].create(rightface, Dir::Ind3d::Left, this);
			faces[4].create(frontface, Dir::Ind3d::Front, this);
			faces[5].create(backface, Dir::Ind3d::Back, this);
			break;
			case Dir::Ind2d::Down://block faces backwords
			faces[0].create(rightface, Dir::Ind3d::Right, this);
			faces[1].create(leftface, Dir::Ind3d::Left, this);
			faces[4].create(backface, Dir::Ind3d::Front, this);
			faces[5].create(frontface, Dir::Ind3d::Back, this);
			break;

			}

		};

		bool transparent;
	private:
		friend struct face;
		dirty_flag& flag;

	};
	inline v3::Point3 face::center() const {
		return facenum.to_vec() * mesh->box.scale + mesh->box.center;
	}
	inline void face::set_cover(cover_state new_state) {
		mesh->flag.mark_dirty();
		cover=new_state;
	}
	inline void face::set_light(size_t value) {
		mesh->flag.mark_dirty();
		light = value;
	}
}