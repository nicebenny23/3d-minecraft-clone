#include "renderer.h"
#include "../game/GameContext.h"
#include "../game/Core.h"
#include "../game/ecs/unique_object.h"
#include "../game/ecs/query.h"
#include "../math/meshes.h"
#pragma once
namespace decals {
	struct DecalReimageCommand {
		DecalReimageCommand(const ecs::obj& comp, const renderer::TexturePath& texture_path)
			:decal_comp(comp), path(texture_path) {

		}
		ecs::obj decal_comp;
		renderer::TexturePath path;
	};
	struct decal_component :ecs::component {
		v3::Point3 center;
		decal_component(v3::Point3 decal_center):center(decal_center),tangent(v3::zerov),bi_tangent(v3::zerov),normal(v3::zerov),handle() {

		}
		v3::Vec3 tangent;
		v3::Vec3 bi_tangent;
		v3::Vec3 normal;
		void destroy_hook() {
			handle.destroy();
		}
		void enable() {
			if (handle) {
				handle.enable();
			}
		}
		void disable() {
			if (handle) {
				handle.disable();
			}
		}
		renderer::RenderableHandle handle;
		void set_handle(const renderer::TexturePath& path) {
			world().emplace_command<DecalReimageCommand>(owner(),path);
		}
	};

	
	struct DecalSpawner :ecs::Recipe {
		v3::Point3 pnt;
		DecalSpawner(v3::Point3 center):pnt(center){
			
		}
		void apply(ecs::obj& entity) {
			entity.add_component<decal_component>(pnt);
		}
	};

	struct render_decals :ecs::System {
		render_decals() {

		};
		void run(ecs::Ecs& world) {

			for (DecalReimageCommand& cmd : world.read_commands<DecalReimageCommand>()) {
				if (cmd.decal_comp.exists()) {
					decal_component& dec = cmd.decal_comp.get_component<decal_component>();
					if (!dec.handle) {
						dec.handle = world.get_resource<renderer::Renderer>().unwrap().gen_renderable();
						dec.handle.set_material("decal_mat");
						dec.handle.set_layout(vertice::vertex().push<float, 3>().push<float, 2>());
					}
					dec.handle.set_uniform(renderer::uniform(world.load_asset_emplaced<renderer::TexturePath>(cmd.path).unwrap(), "tex"));
				}
			}

			ecs::View<decal_component> view = ecs::View<decal_component>(world);
			for (auto[dec] : view) {
				
				if (dec.handle) {


					renderer::MeshData mesh = dec.handle.create_mesh();
					for (int i = 0; i < 4; i++) {
						v2::Vec2 norm_uv = math::symetrical_square_mesh[i];
						const double eps = .001;
						//brings it above the surface
						v3::Point3 point = dec.normal * eps + dec.center + dec.tangent * norm_uv.x + dec.bi_tangent * norm_uv.y;
						mesh.add_point(point, math::square_mesh[i]);
					}
					mesh.add_indices(math::square_mesh_triangle_indices);
					dec.handle.fill(std::move(mesh));
					dec.handle.set_order_key(dist(camera::GetCam().position, dec.center));
				}
			}
		}
	};
	struct decal_plugin :Core::Plugin {
		void build(Core::App& engine) {
			engine.emplace_system<render_decals>();
			renderer::shader_id decal_shader = CtxName::ctx.Ecs->load_asset_emplaced<renderer::shader_descriptor>("decal_shader", "shaders\\decal_vert.vs", "shaders\\decal_frag.vs").unwrap();
			engine.Ecs.load_asset_emplaced<renderer::MaterialDescriptor>("decal_mat", "transparent_phase", "decal_shader", renderer::RenderProperties(true, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
				stn::array{ renderer::uparam("aspect_ratio", "aspectratio"),
				renderer::uparam("proj_matrix", "projection"),
				renderer::uparam("view_matrix", "view") }
			);
		}

	};
}