#pragma once
#include "../math/vector3.h"
#include "../math/dir.h"
#include "../math/geometry.h"
#include "../util/cached.h"
#include "block_registry.h"
#include "../math/cube_vertex.h"
namespace blocks {

	struct BlockMesh;
	enum class cover_state :char {
		Covered = 0,
		Uncovered = 1,
		Uncomputed = 2

	};

	struct BlockFace {


		bool uncomputed() const {
			return cover == cover_state::Uncomputed;
		}
		bool uncovered() const {
			return cover == cover_state::Uncovered;
		}
		bool covered() const {
			return cover == cover_state::Covered;
		}

		BlockFace(block_texture texval)
			:tex(texval), cover(cover_state::Uncomputed) {
		}
		block_texture tex;
		cover_state cover;
	};
	struct MeshFace {

		BlockFace& face();
		v3::Point3 center();
		bool uncovered() {
			return face().uncovered();
		}
		bool uncomputed() {
			return face().uncomputed();
		}
		bool covered() {
			return face().covered();
		}

		BlockMesh& mesh();
		math::Direction3d direction() const{
			return face_direction;
		}
		MeshFace(BlockMesh& mesh, math::Direction3d BlockFace) :face_mesh(mesh), face_direction(BlockFace) {
			
		}
	private:
		stn::non_null<BlockMesh> face_mesh;
		math::Direction3d face_direction;
	};
	
	struct BlockMesh {
	private:
		geo::Box box;

	public:
		//the vertex at the lowest corner of the world
		stn::List<BlockFace, 6> faces;
		math::Direction3d attached_direction;
		math::Direction2d direction;

		geo::Box bounds() const {
			return box;
		}
		v3::Point3 center() const {
			return box.center;
		}
		bool is_transparent() const {
			return transparent;
		}
		//we should make a better mechanism to do this
		bool invisible() const {
			return air_like;
		}
		bool visible() const {
			return !invisible();
		}
		BlockMesh(const BlockMeshTraits& textures, stn::dirty_flag& d_flag, v3::Coord position, math::Direction3d attachment_direction, math::Direction2d facing_direction) :
			flag(d_flag), box(position + unitv / 2, textures.size), transparent(false), faces([&](size_t index) {
			return BlockFace(textures.faces[index]);
				}), attached_direction(attachment_direction)
					, direction(facing_direction), air_like(textures.invisible) {
			box.center -= attached_direction.vec() * box.scale.shrunk(1)/2;
		}
		MeshFace operator[](math::Direction3d index) {
			return MeshFace(*this, index);
		}

		MeshFace operator[](size_t index) {
			return MeshFace(*this, math::Direction3d(math::DirectionIndex3d(index)));
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

		void mark_dirty(math::Direction3d index_face) {
				BlockFace& face = faces[index_face.index()];
				flag.mark_dirty();
				face.cover = cover_state::Uncomputed;
		}
		
		friend struct BlockFace;
		stn::dirty_flag& flag;

	};
	inline BlockFace& MeshFace::face() {
		return face_mesh->faces.unchecked_at(face_direction.index());

	}

	inline v3::Point3 MeshFace::center() {
		return mesh().bounds().in_direction(face_direction);
	}
	inline BlockMesh& MeshFace::mesh() {
		return *face_mesh;
	}
}