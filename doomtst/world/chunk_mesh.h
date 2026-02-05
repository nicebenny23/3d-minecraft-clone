#include "../renderer/renderer.h"
#include "../block/block_mesh.h"
#include "../util/cached.h"
#include "../game/camera.h"
#pragma once
namespace Chunk {

	constexpr int chunklength = 16;
	struct chunkmesh :ecs::component {
		chunkmesh() :recreate_mesh(true) {
		};
		void start() {
			renderer::Renderer& ren = world().get_resource<renderer::Renderer>().expect("renderer must exist");
			SolidGeo = ren.gen_renderable();
			SolidGeo.set_material("SolidBlock");
			SolidGeo.set_layout(vertice::vertex().push<float, 3>().push<float, 3>().push<float, 1>());
			TransparentGeo = ren.gen_renderable();
			TransparentGeo.set_material("TransparentBlock");
			TransparentGeo.set_layout(vertice::vertex().push<float, 3>().push<float, 3>().push<float, 1>());
		}

		renderer::RenderableHandle SolidGeo;
		renderer::RenderableHandle TransparentGeo;

		stn::dirty_flag recreate_mesh;
		v3::Coord loc;

		Point3 center() const {
			return (loc + unitv / 2.f) * chunklength;
		}
		array<blocks::face> faces;
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