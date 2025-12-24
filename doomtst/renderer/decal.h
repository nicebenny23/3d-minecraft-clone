#include "renderer.h"
#include "../game/GameContext.h"
#include "../game/Core.h"
#pragma once

struct decal {
	v3::Point3 center;
	
	v3::Vec3 tangent;
	v3::Vec3 bi_tangent;
	v3::Vec3 normal;
	void destroy() {
		handle.destroy();
	}
	
	renderer::RenderableHandle handle;
	void create_mesh();
	void remove();
	~decal() {
		remove();
	}
	void set_handle(const char* texloc, const char* texture);
	void render();
};

inline void create_decal_material() {

	CtxName::ctx.Ren->Shaders.Compile("decal_shader", "shaders\\decal_vert.vs", "shaders\\decal_frag.vs");
	CtxName::ctx.Ren->construct_material("decal_mat", "decal_shader", renderer::RenderProperties(true, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
		uniforms::uparam("aspect_ratio", "aspectratio"),
		uniforms::uparam("proj_matrix", "projection"),
		uniforms::uparam("view_matrix", "view")
	);


}
struct decal_system :ecs::resource {

	stn::array<renderer::RenderableHandle> decals;
	void add(renderer::RenderableHandle handle) {
		if (!decals.contains(handle)) {
			decals.push(handle);
		}

	}
	void remove(renderer::RenderableHandle handle) {
		Option<size_t> index = decals.find(handle);
		if (index) {
			decals.remove_at(index.unwrap());
		}
	}
};
struct render_decals :ecs::System {
	render_decals() {

	};
	void run(ecs::Ecs& world) {
		for (auto& dec : world.ensure_resource<decal_system>().decals) {
			dec.render();
		}
	}
};
struct decal_plugin:Core::Plugin{
	void Build(Core::Engine& engine) {
		engine.emplace_system<render_decals>();
		engine.emplace_resource<decal_system>();

	}

};
