#include "decal.h"


const v2::Vec2 cubeuv[] = {
	v2::Vec2(0, 1),
	v2::Vec2(1, 1),
	v2::Vec2(0, 0),
	v2::Vec2(1, 0)
};
void decal::set_handle(const char* texloc, const char* texture) {
	if (!handle) {

		handle = CtxName::ctx.Ren->gen_renderable();
		handle.set_material("decal_mat");
		handle.set_layout(vertice::vertex().push<float, 3>().push<float, 2>());

	}
	handle.set_uniform(uniforms::uniform(CtxName::ctx.Ecs->get_resource<renderer::Renderer>().unwrap().Textures.LoadTexture(texloc, texture), "tex"));

}
void decal::create_mesh() {
	renderer::MeshData mesh = handle.create_mesh();
	for (int i = 0; i < 4; i++) {
		v2::Vec2 norm_uv = (cubeuv[i] - v2::Vec2(.5, .5)) * 2;
		const double eps = .001;
		//brings it above the surface
		v3::Point3 point = normal * eps + center + tangent * norm_uv.x + bi_tangent * norm_uv.y;
		mesh.add_point(point, cubeuv[i]);
	}
	mesh.add_indices({ 0,1,3,0,3,2 });
	handle.fill(std::move(mesh));
	CtxName::ctx.Ren->pop();

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


