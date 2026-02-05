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
		minecrafttorch = 4,
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
		math::Direction3d face_direction;
		bool uncomputed() const {
			return cover == cover_state::Uncomputed;
		}
		bool uncovered() const {
			return cover == cover_state::Uncovered;
		}
		bool covered() const {
			return cover == cover_state::Covered;
		}
		
		blockmesh* mesh;
		face(block_textures texval, math::Direction3d num, blockmesh* owner)
		:face_direction(math::Direction3d(num)),mesh(owner),tex(texval),cover(cover_state::Uncomputed),light(0){}
		inline Point3 center() const;
		size_t get_light() const{
			return light;
		}

		void set_cover(cover_state new_state);
		void set_light(std::uint8_t  value);
	private:
		cover_state cover;
		std::uint8_t light;
	};

	struct blockmesh {
		math::Direction2d direction;
		math::Direction3d attached_direction;
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
		blockmesh(const stn::List<block_textures,6>& textures,dirty_flag& d_flag,v3::Coord position, Scale3 blkscale,math::Direction3d attachment_direction,math::Direction2d facing_direction) :
			flag(d_flag), box(position + unitv / 2, blkscale), mesh_location(position),transparent(false),faces([&](size_t index) { 
				return face(textures[index],math::Direction3d(math::DirectionIndex3d(index)), this);
			}), attached_direction(attachment_direction),direction(facing_direction)
		{

			box.center -= attached_direction.vec()*box.scale.shrunk(.5f);
		}
		geo::Box box;
		Coord mesh_location;
		stn::List<face,6> faces;
		face& operator[](size_t index) {
			return faces[index];
		}
		const face& operator[](size_t index) const {
			return faces[index];
		}

		face& operator[](math::AxisIndex3d index) {
			return faces.unchecked_at(static_cast<size_t>(index));
		}
		const face& operator[](math::AxisIndex3d index) const{
			return faces.unchecked_at(static_cast<size_t>(index));
		}
		using iterator = decltype(faces)::iterator;
		iterator begin() {
			return faces.begin();
		}
		iterator end() {
			return faces.end();
		}


		
	
		bool transparent;
		friend struct face;
		dirty_flag& flag;

	};
	inline v3::Point3 face::center() const {
		return face_direction.vec() * mesh->box.scale + mesh->box.center;
	}
	inline void face::set_cover(cover_state new_state) {
		mesh->flag.mark_dirty();
		cover=new_state;
	}
	inline void face::set_light(std::uint8_t value) {
		mesh->flag.mark_dirty();
		light = value;
	}
}