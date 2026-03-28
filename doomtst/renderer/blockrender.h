#include "../world/managegrid.h"

#include "../game/Core.h"
#include "../math/geometry.h"
#include "../math/intersection.h"
#include "../util/thread_split.h"

#include "../block/block.h"
#include "../block/block.h"
#include "../block/block_mesh.h"
#include "../block/block_mesh.h"
#include "../block/block_registry.h"
#include "../block/block_registry.h"
#include "../game/camera.h"
#include "../game/camera.h"
#include "../game/ecs/ecs.h"
#include "../game/ecs/ecs.h"
#include "../game/ecs/game_object.h"
#include "../game/ecs/game_object.h"
#include "../game/ecs/system.h"
#include "../game/ecs/system.h"
#include "../math/cube_vertex.h"
#include "../math/cube_vertex.h"
#include "../math/dir.h"
#include "../math/dir.h"
#include "../math/meshes.h"
#include "../math/ray.h"
#include "../math/ray.h"
#include "../math/Scale3.h"
#include "../math/Scale3.h"
#include "../math/vector2.h"
#include "../math/vector2.h"
#include "../math/vector3.h"
#include "../math/vector3.h"
#include "../player/cameracomp.h"
#include "../player/cameracomp.h"
#include "../util/dynamicarray.h"
#include "../util/dynamicarray.h"
#include "../util/exception.h"
#include "../util/exception.h"
#include "../world/chunk.h"
#include "../world/chunk.h"
#include "../world/chunk_mesh.h"
#include "../world/chunk_mesh.h"
#include "../world/grid.h"
#include "../world/grid.h"
#include "../world/Lighter.h"
#include "../world/Lighter.h"
#include "../world/WorldCoverer.h"
#include "../world/WorldCoverer.h"
#include "renderer.h"
#include "renderer.h"
#include "renderer/RenderContext.h"
#include "renderer/RenderContext.h"
#include "renderer/RenderProperties.h"
#include "renderer/RenderProperties.h"
#include "renderer/shader.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/texture.h"
#include "renderer/Uniform.h"
#include "renderer/Uniform.h"
#include <cmath>
#include <cmath>
#include <glad/glad.h>
#include <glad/glad.h>
#include <mutex>
#include <string>
#include <string>
#include <utility>
#include <utility>
#pragma once 
namespace blockrender {
	using namespace grid;



	inline bool chunk_viewable(Chunk::chunk& chk) {
		return true;
		//does not seem to be working
		float slope = tan(chk.world().get_resource<renderer::camera_resource>().camera.get<renderer::CameraComponent>().fov / 2);
		math::Box chkb = chk.bounds();
		math::ray camray = math::ray::from_offset(camera::campos(), camera::GetCamFront());
		math::cone ncone = math::cone(camray, slope);
		math::Plane pln = math::Plane(camera::GetCamFront(), camray.start);
		bool srf = false;
		for (int i = 0; i < 8; i++) {
			Point3 vertex = chk.center().offset_local((math::cube_mesh[i] - unitv / 2.f) * float(Chunk::chunk_length));
			if (dot(camera::campos() - vertex, camera::GetCamFront()) > 0) {
				srf = true;
			}
		}

		if (!srf) {
			return false;
		}
		return geointersect::intersects(ncone, math::Sphere(chkb));

	}

	// Calculate UV coordinates for a face centered at the mesh
	v2::Vec2 face_to_uv_coords(const block_mesh& mesh, const size_t index, size_t uv_index) {
		const v3::Scale3& meshscale = mesh.bounds().half_size();
		size_t facetype = index / 2;
		v2::Vec2 offset;

		switch (facetype) {
		case 0:
		offset = v2::Vec2(meshscale.z, meshscale.y) / blocksize;
		break;
		case 1:
		offset = v2::Vec2(meshscale.x, meshscale.z) / blocksize;
		break;
		case 2:
		offset = v2::Vec2(meshscale.x, meshscale.y) / blocksize;
		break;
		default:
		stn::throw_logic_error("axis index is always less than 3");
		}

		v2::Vec2 uvCoord = math::square_mesh[uv_index];
		v2::Vec2 ret = v2::unitv / 2 + offset * (v2::unitv - uvCoord * 2);
		return ret;
	}
	template<WorldAccessor Accesor>
	void emit_face(Accesor& world, blocks::MeshFace mesh_face, renderer::MeshData& render_mesh) {
		blocks::face& face = mesh_face.face();
		if (face.uncovered()) {

			const int baselocation = render_mesh.vertex_count();
			const int* uniqueInds = &math::cube_mesh_face_indices[4 * face.face_direction.index()];
			Scale3 scale = mesh_face.mesh().bounds().half_size();
			Point3 position = mesh_face.mesh().center();
			const int textureNumber = face.tex;
			for (size_t j = 0; j < 4; j++) {
				int uniqueind = uniqueInds[j];
				v3::Coord cube_ind = math::cube_mesh[uniqueind];
				v3::Point3 offset_from_center = (v3::Point3(cube_ind) - unitv / 2) * scale * 2;
				Point3 offset = position.offset_local(offset_from_center);

				v2::Vec2 uv_coords = face_to_uv_coords(mesh_face.mesh(), face.face_direction.index(), j);

				math::cube_index face_cube = math::cube_index::from_coord(cube_ind).expect("cube ind should be valid");
				float light_value = grid::get_vertex_light(world.get_voxel(mesh_face.mesh().center() + face_cube.vertex()), world);
				render_mesh.add_point(offset, uv_coords, textureNumber, light_value);
			}
			render_mesh.add_indices_offset_from(math::square_mesh_triangle_indices, baselocation);
		}

	}
	// Recreate the mesh for a chunk
	void recreate_chunk_mesh(grid::ChunkObject::ObjectType chunk_to_fill, std::mutex& fill_lock) {
		grid::Grid& grid = chunk_to_fill.world().get_resource<grid::Grid>();
		Chunk::chunk& cnk = chunk_to_fill.get<Chunk::chunk>();
		Chunk::chunkmesh& cnk_mesh = chunk_to_fill.get<Chunk::chunkmesh>();
		grid::FocusedGridAcessor chunk_getter(chunk_to_fill);
		cnk_mesh.recreate_mesh.clean([&]() {

			cnk_mesh.faces.clear();
			renderer::MeshData mesh = cnk_mesh.SolidGeo.create_mesh(vertice::vertex().push<float, 3>().push<float, 3>().push<float, 1>());
			for (Chunk::block_object& obj: cnk) {
				block_mesh& mesh_at = (obj.get_unchecked<block>().mesh);//g

				if (mesh_at.invisible()) {
					continue;
				}
				compute_mesh_cover(mesh_at, chunk_getter);
				if (!mesh_at.is_transparent()) {
					for (math::Direction3d dir : math::Directions3d) {
						emit_face(chunk_getter, mesh_at[dir], mesh);
					}
				}
				else {
					for (math::Direction3d dir : math::Directions3d) {
						if (mesh_at[dir].uncovered()) {
							cnk_mesh.faces.push(mesh_at[dir]);
						}
					}
				}
			}
			{
				std::unique_lock lck(fill_lock);
				cnk_mesh.SolidGeo.fill(std::move(mesh));
			}
			});
	}



	// Render a chunk mesh
	void render_chunk(grid::Grid& world, Chunk::chunkmesh& mesh) {
		double distance = v3::dist(camera::GetCam().position, mesh.center());
		mesh.SolidGeo.set_order_key(distance);
		mesh.sort_faces();
		renderer::MeshData mesh_data = mesh.TransparentGeo.create_mesh(vertice::vertex().push<float, 3>().push<float, 3>().push<float, 1>());
		for (int i = 0; i < mesh.faces.length(); i++) {

			emit_face(world, mesh.faces[i], mesh_data);
		}
		mesh.TransparentGeo.set_order_key(-distance);
		mesh.TransparentGeo.fill(std::move(mesh_data));
	}

	struct ChunkPreMesher :ecs::System {
		ChunkPreMesher() {

		}

		void run(ecs::Ecs& ecs) {
			grid::Grid& world_grid = ecs.get_resource<grid::Grid>();			GlUtil::poll_errors();

			ecs.get_resource<renderer::Renderer>().set_uniform("render_dist", int(world_grid.rad));
			std::mutex fill_mutex;
			auto recompute_for = [&fill_mutex](grid::ChunkObject::ObjectType& item) {

				recreate_chunk_mesh(grid::ChunkObject::ObjectType(item), fill_mutex);
				};
			stn::array<grid::ChunkObject::ObjectType> reloads;
			ecs::View<ecs::With<Chunk::chunkmesh>, ecs::Owner> meshes(ecs);
			size_t max_recomputes = 4;
			for (auto&& [mesh, object] : meshes) {
				if (mesh.recreate_mesh.is_dirty()) {
					if (chunk_viewable(object.get_component<Chunk::chunk>())) {
						reloads.push(object);
					}
				}
				if (reloads.length() == max_recomputes) {
					break;
				}
			}
			thread_util::par_iter(reloads.begin(), reloads.end(), recompute_for, reloads.length());
		};
	};

	struct ChunkRenderer :ecs::System {
		ChunkRenderer() {
		}
		void run(ecs::Ecs& ecs) {
			array<ecs::obj> to_render = array<ecs::obj>();
			Grid& grid = ecs.get_resource<grid::Grid>();
			ecs::View<ecs::With<Chunk::chunkmesh>> meshes(ecs);
			for (auto&& [mesh] : meshes) {
				render_chunk(grid, mesh);

			}

		}
	};
	struct BlockRenderPlugin :Core::Plugin {

		void build(Core::App& engine) {
			engine.emplace_system<grid::GridManager>();
			engine.emplace_system<grid::GridCoverer>();
			engine.emplace_system<grid::LightRemover>();
			engine.emplace_system<grid::GridLighter>();
			engine.emplace_system<ChunkPreMesher>();
			engine.emplace_system<ChunkRenderer>();
			initblockrendering(engine.Ecs);
		}
		void initblockrendering(ecs::Ecs& ecs) {

			auto& renderer = ecs.get_resource<renderer::Renderer>();
			renderer::shader_id block_shader = ecs.load_asset_emplaced<renderer::shader_descriptor>("BlockShader", "shaders\\vert1.vs", "shaders\\frag1.vs").unwrap();
			ecs.load_asset_emplaced<renderer::MaterialDescriptor>("SolidBlock", "solid_phase", "BlockShader", renderer::RenderProperties(true, true, false, false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
				stn::array{
				renderer::UniformRefrence("aspect_ratio", "aspectratio"),
				renderer::UniformRefrence("proj_matrix", "projection"),
				renderer::UniformRefrence("view_matrix", "view"),
				renderer::UniformRefrence("camera_pos", "camera_pos"),
				renderer::UniformRefrence("render_dist", "render_distance"),
				renderer::UniformRefrence("view_matrix", "view"),
				renderer::UniformRefrence("bind_block_texture", "tex")
				});

			ecs.load_asset_emplaced<renderer::MaterialDescriptor>("TransparentBlock", "transparent_phase", "BlockShader", renderer::RenderProperties(true, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
				stn::array{ renderer::UniformRefrence("aspect_ratio", "aspectratio"),
				renderer::UniformRefrence("proj_matrix", "projection"),
				renderer::UniformRefrence("view_matrix", "view"),
				renderer::UniformRefrence("bind_block_texture", "tex")
				});

			array<std::string> texlist = array<std::string>();
			texlist.reach(treestonetex) = "images\\treestone.png";
			texlist.reach(grasstex) = "images\\grass.png";
			texlist.reach(stonetex) = "images\\stone.png";
			texlist.reach(altartex) = "images\\crystalaltarside.png";
			texlist.reach(glasstex) = "images\\glass.png";
			texlist.reach(watertex) = "images\\water.png";
			texlist.reach(torchtex) = "images\\torch.png";
			texlist.reach(torchtoptex) = "images\\torchtop.png";
			texlist.reach(crystaloretex) = "images\\crystalore.png";
			texlist.reach(chest_top) = "images\\chest_top.png";
			texlist.reach(chest_front) = "images\\chest.png";
			texlist.reach(chest_sides) = "images\\chest_sides.png";
			texlist.reach(crystaltorchtoptex) = "images\\crystaltorchtop.png";
			texlist.reach(mosstex) = "images\\moss.png";
			texlist.reach(ropetex) = "images\\rope.png";
			texlist.reach(lavatex) = "images\\lava.png";
			texlist.reach(obsidiantex) = "images\\obb.png";
			texlist.reach(crafting_table_front) = "images\\craftingtable.png";
			texlist.reach(crafting_table_side) = "images\\craftingtableside.png";
			texlist.reach(furnacefront) = "images\\furnacetop.png";
			texlist.reach(furnaceside) = "images\\furnace.png";
			texlist.reach(ironoretex) = "images\\ironore.png";
			texlist.reach(furnacesideon) = "images\\furnaceon.png";
			texlist.reach(furnacefronton) = "images\\furnacetopon.png";
			texlist.reach(logtoppng) = "images\\log.png";
			texlist.reach(ultraaltarpngultrapng) = "images\\ultraaltar.png";
			texlist.reach(sandtex) = "images\\sand.png";
			texlist.reach(planktex) = "images\\treestoneblock.png";
			renderer::texture_array_id texarray = ecs.load_asset_emplaced<renderer::TextureArrayPath>(texlist, "BlockTextures").unwrap();
			renderer.Bind_texture(texarray);
			renderer.set_uniform("bind_block_texture", texarray);
		}
	};
}
