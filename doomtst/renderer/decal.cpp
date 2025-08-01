#include "decal.h"
struct decal_system {

    stn::array<renderer::RenderableHandle> decals;

};
decal_system decals;

const v2::Vec2 cubeuv[] = {
    v2::Vec2(0, 1),
    v2::Vec2(1, 1),
    v2::Vec2(0, 0),
    v2::Vec2(1, 0)
};
void decal::create_handle(const char* texloc, const char* texture)
{
    if (!handle)
    {

        handle = CtxName::ctx.Ren->gen_renderable();
        int l = 2;

    }handle.set_material("decal_mat");
    handle.set_uniform(uniforms::uniform(handle.renderer->Textures.LoadTexture(texloc, texture), "tex"));
    handle.set_layout(vertice::vertex().push<float, 3>().push<float,2>());
    
}
void decal::create_mesh(){
    renderer::MeshData mesh = handle.create_mesh();

for (int  i = 0; i < 4; i++)
{
    v2::Vec2 norm_uv = (cubeuv[i] - v2::Vec2(.5, .5)) * 2;
    v3::Vec3 point =normal*.001+center+ norm_uv.x * tangent + norm_uv.y * bi_tangent;
    mesh.add_point(point, cubeuv[i]);
}
mesh.add_index(0);
mesh.add_index(1);
mesh.add_index(3);
mesh.add_index(0);
mesh.add_index(3);
mesh.add_index(2);
handle.fill(std::move( mesh));
handle.renderer->consume();

}
void decal::render() {
    create_mesh();
    decals.decals.push(handle);
}

void render_decals()
{
    for (auto& dec: decals.decals)
    {
        dec.render();
    }
    decals.decals.destroy();
}
