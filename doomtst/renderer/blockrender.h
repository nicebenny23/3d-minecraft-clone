#include "../world/managegrid.h"

#include "../game/Core.h"
#include "../math/geometry.h"
#include "../math/intersection.h"
#include "../util/thread_split.h"

#include "../block/block.h"
#include "../block/block_mesh.h"
#include "../block/block_registry.h"
#include "../game/ecs/ecs.h"
#include "../game/ecs/ecs.h"
#include "../game/ecs/game_object.h"
#include "../game/ecs/system.h"
#include "../math/cube_vertex.h"
#include "../math/dir.h"
#include "../math/meshes.h"
#include "../math/ray.h"
#include "../math/vector2.h"
#include "../math/vector3.h"
#include "../math/vector3.h"
#include "../player/cameracomp.h"
#include "../util/dynamicarray.h"
#include "../util/exception.h"
#include "../world/chunk.h"
#include "../world/chunk_mesh.h"
#include "../world/grid.h"
#include "../world/Lighter.h"
#include "../world/WorldCoverer.h"
#include "renderer.h"
#include "renderer/RenderContext.h"
#include "renderer/RenderProperties.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/Uniform.h"
#include <cmath>
#include <glad/glad.h>
#include <mutex>
#include <string>
#include <utility>
#pragma once 
namespace blocks {
	using namespace grid;



	inline bool chunk_viewable(chunks::Chunk& chk) {
		geo::Frustum view = renderer::world_camera_frustum(chk.world());
		return geo::frustum_box_intersection(view, chk.bounds());

	}

	// Calculate UV coordinates for a BlockFace centered at the mesh
	v2::Vec2 face_to_uv_coords(const BlockMesh& mesh, const size_t index, size_t uv_index) {
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
	void emit_face(Accesor& world, blocks::MeshFace mesh_face, renderer::MeshBuilder& render_mesh) {
		blocks::BlockFace& face = mesh_face.face();
		if (face.uncovered()) {

			const int baselocation = render_mesh.vertex_count();
			const int* uniqueInds = &math::cube_mesh_face_indices[4 * mesh_face.direction().index()];
			Scale3 scale = mesh_face.mesh().bounds().half_size();
			if (mesh_face.mesh().transparent) {
				scale[size_t(mesh_face.direction().axis())] *= .999;
			}
			Point3 position = mesh_face.mesh().center();
			const int textureNumber = face.tex;
			for (size_t j = 0; j < 4; j++) {
				int unique_ind = uniqueInds[j];
				v3::Coord cube_ind = math::cube_mesh[unique_ind];
				v3::Point3 offset_from_center = (v3::Point3(cube_ind) - unitv / 2) * scale * 2;
				Point3 offset = position + offset_from_center;

				v2::Vec2 uv_coords = face_to_uv_coords(mesh_face.mesh(), mesh_face.direction().index(), j);

				math::cube_index face_cube = math::cube_index::from_coord(cube_ind).expect("cube ind should be valid");
				float light_value = grid::get_vertex_light(world.get_voxel(mesh_face.mesh().center() + face_cube.vertex()), world);
				render_mesh.add_point(offset, uv_coords, textureNumber, light_value);
			}
			render_mesh.add_indices_offset_from(math::square_mesh_triangle_indices, baselocation);
		}

	}
	// Recreate the mesh for a chunk
	void recreate_chunk_mesh(grid::ChunkObject::ObjectType chunk_to_fill, std::mutex& fill_lock) {

		timing::TimeProfiler profiler = timing::TimeProfiler();

		grid::Grid& grid = chunk_to_fill.world().get_resource<grid::Grid>();
		chunks::Chunk& cnk = chunk_to_fill.get<chunks::Chunk>();
		chunks::ChunkMesh& cnk_mesh = chunk_to_fill.get<chunks::ChunkMesh>();
		grid::FocusedGridAcessor chunk_getter(chunk_to_fill);

		cnk_mesh.recreate_mesh.clean([&]() {

			cnk_mesh.faces.clear();
			renderer::MeshBuilder mesh(cnk_mesh.solid.mesh().unwrap(), renderer::vertex().push<float, 3>().push<float, 3>().push<float, 1>());
			for (chunks::block_object& obj : cnk) {
				BlockMesh& mesh_at = (obj.get_unchecked<block>().mesh);//g

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
				renderer::fill(std::move(mesh), cnk_mesh.world());
			}
			});

		double val = profiler.milliseconds();
	}



	// Render a chunk mesh
	inline void render_chunk(grid::Grid& world, ecs::Constrained<chunks::ChunkMesh> mesh_object) {
		chunks::ChunkMesh& mesh = mesh_object.get<chunks::ChunkMesh>();
		double distance = v3::dist(mesh.world().get_resource<CameraResource>().center(), mesh.center());
		mesh.solid.set_order_key(distance);
		mesh.sort_faces();
		grid::FocusedGridAcessor accesor(mesh_object.object(), world);
		renderer::MeshBuilder mesh_data = mesh.transparent.insert_builder_for(renderer::vertex().push<float, 3>().push<float, 3>().push<float, 1>());
		for (int i = 0; i < mesh.faces.length(); i++) {

			emit_face(accesor, mesh.faces[i], mesh_data);
		}
		mesh.transparent.set_order_key(-distance);
		renderer::fill(std::move(mesh_data), mesh.world());
	}

	struct ChunkPreMesher :ecs::System {
		ChunkPreMesher() {

		}

		void run(ecs::Ecs& ecs) {
			auto& renderer = ecs.get_resource<renderer::Renderer>();
			BlockTextureRegistry& registry = ecs.insert_resource<blocks::BlockRegistry>().textures;
			if (registry.need_sync()) {
				array<std::string> texlist = array<std::string>();
				for (auto&& value : registry.name_to_texture) {
					texlist.reach(value.second) = value.first;
				}
				renderer::TextureArrayId texarray = ecs.load_asset_emplaced<renderer::TextureArrayPath>(texlist, "BlockTextures").unwrap();
				renderer.set_uniform("bind_block_texture", texarray);
				registry.last_saved = texarray->length;
			}
			grid::Grid& world_grid = ecs.get_resource<grid::Grid>();
			ecs.get_resource<renderer::Renderer>().set_uniform("render_dist", int(world_grid.rad));
			std::mutex fill_mutex;
			auto recompute_for = [&fill_mutex](grid::ChunkObject::ObjectType& item) {

				recreate_chunk_mesh(grid::ChunkObject::ObjectType(item), fill_mutex);
				};
			stn::array<grid::ChunkObject::ObjectType> reloads;
			ecs::View< chunks::ChunkMesh, ecs::Owner> meshes(ecs);
			size_t max_recomputes = 4;
			for (auto&& [mesh, object] : meshes) {
				if (mesh.recreate_mesh.is_dirty()) {
					if (chunk_viewable(object.get_component<chunks::Chunk>())) {
						reloads.push(object);
					}
				}
				if (reloads.length() == max_recomputes) {
					break;
				}
			}
			timing::TimeProfiler profiler{};
			thread_util::par_iter_rng(reloads, recompute_for, reloads.length());
			double value = profiler.milliseconds();
			int l = 2;
			if (25 < value) {
				int l = 3;
			}
		};
	};

	struct ChunkRenderer :ecs::System {
		ChunkRenderer() {
		}
		void run(ecs::Ecs& ecs) {
			size_t total_render_count = 0;
			Grid& grid = ecs.get_resource<grid::Grid>();
			ecs::View< ecs::Constrained<chunks::ChunkMesh>, chunks::Chunk> meshes(ecs);
			for (auto&& [mesh, Chunk] : meshes) {
				if (chunk_viewable(Chunk)) {
					total_render_count++;
					render_chunk(grid, mesh);
				}
			}

		}
	};
	inline void block_render_plugin(core::App& engine) {
		engine.emplace_system<grid::GridManager>();
		engine.emplace_system<grid::GridCoverer>();
		engine.emplace_system<grid::LightRemover>();
		engine.emplace_system<grid::GridLighter>();
		engine.emplace_system<ChunkPreMesher>();
		engine.emplace_system<ChunkRenderer>();
		ecs::Ecs& ecs = engine.Ecs;
		auto& renderer = ecs.get_resource<renderer::Renderer>();
		renderer::shader_id block_shader = ecs.load_asset_emplaced<renderer::shader_descriptor>("BlockShader", "shaders\\vert1.vs", "shaders\\frag1.vs").unwrap();
		ecs.load_asset_emplaced<renderer::MaterialDescriptor>("SolidBlock", "solid_phase", "BlockShader", renderer::RenderProperties(true, true, false, false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
			stn::array{
			renderer::UniformRefrence("proj_matrix", "projection"),
			renderer::UniformRefrence("view_matrix", "view"),
			renderer::UniformRefrence("camera_pos", "camera_pos"),
			renderer::UniformRefrence("render_dist", "render_distance"),
			renderer::UniformRefrence("view_matrix", "view"),
			renderer::UniformRefrence("bind_block_texture", "tex")
			});

		ecs.load_asset_emplaced<renderer::MaterialDescriptor>("TransparentBlock", "transparent_phase", "BlockShader", renderer::RenderProperties(true, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
			stn::array{
			renderer::UniformRefrence("proj_matrix", "projection"),
			renderer::UniformRefrence("view_matrix", "view"),
			renderer::UniformRefrence("bind_block_texture", "tex")
			});
	}
}
