#include "../renderer/renderer.h"
#include "../block/block_mesh.h"
#include "../util/cached.h"
#include "../game/camera.h"
#include "../math/geometry.h"
#include "../math/Scale3.h"
#include "../block/block.h"
#pragma once
namespace Chunks {
	constexpr size_t chunk_length = 16;
	constexpr size_t chunk_axis = size_t(chunk_length / blocksize);
	constexpr size_t chunk_elements = chunk_axis * chunk_axis * chunk_axis;

	constexpr int chunk_shift = std::countr_zero(Chunks::chunk_axis);
	struct ChunkLocation {

		explicit ChunkLocation(v3::Coord pos) :position(pos) {

		}
		static ChunkLocation from_block_pos(v3::Coord pos) {
			pos.x >>= chunk_shift;
			pos.y >>= chunk_shift;
			pos.z >>= chunk_shift;
			return ChunkLocation(pos);
		}
		bool operator==(const ChunkLocation& other) const = default;
		bool operator!=(const ChunkLocation& other) const = default;
		v3::Coord position;
		Point3 center() const {
			return (position + unitv / 2.f) * chunk_length;
		}
		geo::Box bounds() const {
			return geo::Box(center(), v3::Scale3(chunk_length) / 2);
		}
		bool contains_block(v3::Coord block_position) const {
			return from_block_pos(block_position).position==position;
		}
		size_t distance_to(const ChunkLocation& c2) const {
			return v3::manhattan_distance(position, c2.position);
		}
	};

	struct chunkmesh :ecs::component {
		chunkmesh(ChunkLocation location) :loc(location), recreate_mesh(true) {
		};
		void start() {
			renderer::Renderer& ren = world().get_resource<renderer::Renderer>();
			SolidGeo = ren.gen_renderable("SolidBlock");
			
			SolidGeo.mesh();
			TransparentGeo = ren.gen_renderable("TransparentBlock");
			TransparentGeo.mesh();
		}

		renderer::RenderableHandle SolidGeo;
		renderer::RenderableHandle TransparentGeo;

		array<blocks::MeshFace> faces;
		ChunkLocation loc;
		stn::dirty_flag recreate_mesh;
		void mark_dirty() {
			recreate_mesh.mark_dirty();
		}
		Point3 center() const {
			return loc.center();
		}
		geo::Box bounds() const {
			return loc.bounds();
		}
		void sort_faces() {
			v3::Point3 point = world().get_resource<renderer::camera_resource>().world_camera().center();
			std::sort(faces.begin(), faces.end(), [&](blocks::MeshFace& a, blocks::MeshFace& b) {
				return  v3::dist2(b.center(), point) < v3::dist2(a.center(), point);
				});
		}
		void destroy_hook() {
			SolidGeo.destroy();
			TransparentGeo.destroy();
			faces.clear();
		}
	};
}