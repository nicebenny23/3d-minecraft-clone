#include "decal.h"
struct decalsys:ecs::resource {

	stn::array<renderer::RenderableHandle> decals;

};
decalsys decals;

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
	handle.set_uniform(uniforms::uniform(handle.renderer->Textures.LoadTexture(texloc, texture), "tex"));

}
void decal::create_mesh() {
	renderer::MeshData mesh = handle.create_mesh();

	for (int i = 0; i < 4; i++) {
		v2::Vec2 norm_uv = (cubeuv[i] - v2::Vec2(.5, .5)) * 2;
		v3::Point3 point = normal * .001 + center + tangent * norm_uv.x + bi_tangent * norm_uv.y;
		mesh.add_point(point, cubeuv[i]);
	}
	mesh.add_index(0);
	mesh.add_index(1);
	mesh.add_index(3);
	mesh.add_index(0);
	mesh.add_index(3);
	mesh.add_index(2);
	handle.fill(std::move(mesh));
	handle.renderer->pop();

}
void decal::remove() {

	if (handle) {
		Option<size_t> index = decals.decals.find(handle);
		if (index) {
			decals.decals.swap_drop(index.unwrap());
		}
	}
}

void decal::render() {

	create_mesh();
	if (!decals.decals.contains(handle)) {
		decals.decals.push(handle);

	}
}

void render_decals() {
	for (auto& dec : decals.decals) {
		dec.render();
	}
}
