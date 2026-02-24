#pragma once
#include "../math/vector3.h"
#include "../math/dir.h"
#include "../math/geometry.h"
#include "../util/cached.h"
#include "block_registry.h"
namespace blocks {

	struct block_mesh;
	enum class cover_state :char {
		Covered = 0,
		Uncovered = 1,
		Uncomputed = 2

	};
	struct face {


		bool uncomputed() const {
			return cover == cover_state::Uncomputed;
		}
		bool uncovered() const {
			return cover == cover_state::Uncovered;
		}
		bool covered() const {
			return cover == cover_state::Covered;
		}
		
		block_mesh* mesh;
		face(block_textures texval, math::Direction3d num, block_mesh* owner)
		:face_direction(math::Direction3d(num)),mesh(owner),tex(texval),cover(cover_state::Uncomputed),light(0){}
		inline Point3 center() const;
		size_t get_light() const{
			return light;
		}

		void set_cover(cover_state new_state);
		void set_light(std::uint8_t  value);

		block_textures tex;
		math::Direction3d face_direction;
	private:
		cover_state cover;
		std::uint8_t light;
	};

	struct block_mesh {
		stn::List<face, 6> faces;
		geo::Box box;
		math::Direction3d attached_direction;
		math::Direction2d direction;
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
		//we should make a better mechanism to do this
		bool invisible() const {
			return air_like;
		}
		block_mesh(const BlockMeshTraits& textures,stn::dirty_flag& d_flag,v3::Coord position,math::Direction3d attachment_direction,math::Direction2d facing_direction) :
			flag(d_flag), box(position + unitv / 2, textures.size),transparent(false),faces([&](size_t index) {
				return face(textures.faces[index],math::Direction3d(math::DirectionIndex3d(index)), this);
			}), attached_direction(attachment_direction)
			,direction(facing_direction), air_like(textures.invisible)
		{
			box.center -= attached_direction.vec()*box.scale.shrunk(.5f);
		}

			
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
		bool air_like;
		friend struct face;
		stn::dirty_flag& flag;

	};
	inline v3::Point3 face::center() const {
		return face_direction.vec() * mesh->box.scale + mesh->box.center;
	}
	inline void face::set_cover(cover_state new_state) {
		if (new_state!=cover) {
			mesh->flag.mark_dirty();
			cover = new_state;
		}
	}
	inline void face::set_light(std::uint8_t value) {
		if (value!=light) {
			mesh->flag.mark_dirty();
			light = value;
		}
	}
}