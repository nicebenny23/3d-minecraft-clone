#include "renderer.h"
#include "../game/GameContext.h"
#pragma once

struct decal {
	v3::Vec3 center;
	
	v3::Vec3 tangent;
	v3::Vec3 bi_tangent;
	v3::Vec3 normal;

	void destroy() {
		handle.destroy();
	}

	renderer::RenderableHandle handle;
	void create_mesh();
	void create_handle(const char* texloc, const char* texture);
	void render();
};

void render_decals();
inline void create_decal_material() {

	CtxName::ctx.Ren->Shaders.Compile("decal_shader", "shaders\\decal_vert.vs", "shaders\\decal_frag.vs");
	CtxName::ctx.Ren->Construct("decal_mat", "decal_shader", RenderProperties(true, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
		uniforms::uparam("aspect_ratio", "aspectratio"),
		uniforms::uparam("proj_matrix", "projection"),
		uniforms::uparam("view_matrix", "view")
	);


}