#include "../renderer/renderer.h"
#include "../block/block_mesh.h"
#include "../util/cached.h"
#include "../game/camera.h"
#include "../math/geometry.h"
#include "../math/Scale3.h"
#include "../block/block.h"
#pragma once
namespace Chunk {
	constexpr size_t chunk_length = 16;
	constexpr size_t chunk_axis = size_t(chunk_length / blocksize);
	constexpr size_t chunk_elements = chunk_axis * chunk_axis * chunk_axis;
	struct ChunkLocation {

		ChunkLocation(v3::Coord pos) :position(pos) {

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
			return bounds().contains(block_position+v3::unitv/2);
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
			SolidGeo.set_layout(vertice::vertex().push<float, 3>().push<float, 3>().push<float, 1>());
			TransparentGeo = ren.gen_renderable("TransparentBlock");
			
			TransparentGeo.set_layout(vertice::vertex().push<float, 3>().push<float, 3>().push<float, 1>());
		}

		renderer::RenderableHandle SolidGeo;
		renderer::RenderableHandle TransparentGeo;

		array<blocks::face> faces;
		ChunkLocation loc;
		stn::dirty_flag recreate_mesh;

		Point3 center() const {
			return loc.center();
		}
		geo::Box bounds() const {
			return loc.bounds();
		}
		void sort_faces() {

			std::sort(faces.begin(), faces.end(), [](blocks::face& a, blocks::face& b) {
				return  v3::dist2(b.center(), camera::campos()) < v3::dist2(a.center(), camera::campos());
				});
		}
		void destroy_hook() {
			SolidGeo.destroy();
			TransparentGeo.destroy();
			faces.clear();
		}
	};
}