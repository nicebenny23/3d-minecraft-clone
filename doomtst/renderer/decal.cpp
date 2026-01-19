#include "decal.h"
#include "../player/player.h"
#include "../math/meshes.h"

void decal::set_handle(const char* texloc, const char* texture) {
	if (!handle) {

		handle = CtxName::ctx.Ren->gen_renderable();
		handle.set_material("decal_mat");
		handle.set_layout(vertice::vertex().push<float, 3>().push<float, 2>());

	}
	handle.set_uniform(renderer::uniform(CtxName::ctx.Ecs->load_asset_emplaced<renderer::TexturePath>(texloc, texture).unwrap(), "tex"));

}
void decal::create_mesh() {
	renderer::MeshData mesh = handle.create_mesh();
	for (int i = 0; i < 4; i++) {
		v2::Vec2 norm_uv = math::symetrical_square_mesh[i];
		const double eps = .001;
		//brings it above the surface
		v3::Point3 point = normal * eps + center + tangent * norm_uv.x + bi_tangent * norm_uv.y;
		mesh.add_point(point, math::square_mesh[i]);
	}
	mesh.add_indices(math::square_mesh_triangle_indices);
	handle.fill(std::move(mesh));
	handle.set_order_key(dist(camera::GetCam().position, center));

}
void decal::remove() {

	if (handle) {
		CtxName::ctx.Ecs->ensure_resource<decal_system>().remove(handle);
	}
}

void decal::render() {
	create_mesh();
	CtxName::ctx.Ecs->ensure_resource<decal_system>().add(handle);
}


