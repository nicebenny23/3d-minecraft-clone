#include "../world/managegrid.h"
#include "../util/algorthm.h"
#include "../game/Core.h"
#include "../util/thread_split.h"
#include "../math/geometry.h"
#include "../math/intersection.h"
#include "../game/GameContext.h"
#include <mutex>
#include "../math/meshes.h"
#pragma once 
namespace blockrender {
	using namespace grid;



	inline bool chunk_viewable(Chunk::chunk& chk) {
		float slope = tan(chk.world().get_resource<renderer::Renderer>().unwrap().fov / 2);
		geo::Box chkb = chk.span();
		ray camray = ray::from_offset(camera::campos(), camera::GetCamFront());
		geo::cone ncone = geo::cone(camray, slope);
		geo::Plane pln = geo::Plane(camera::GetCamFront(), camray.start);
		bool srf = false;
		for (int i = 0; i < 8; i++) {
			Point3 vertex = chk.center() + (math::cube_mesh[i] - unitv / 2.f) * float(Chunk::chunklength);
			if (dot(camera::campos()-vertex , camera::GetCamFront()) > 0) {
				srf = true;
			}
		}

		if (!srf) {
			return false;
		}
		return geointersect::intersects(ncone, geo::Sphere(chkb));

	}

	// Calculate UV coordinates for a face centered at the mesh
	v2::Vec2 face_to_uv_coords(const blockmesh& mesh, const size_t index, size_t uv_index) {
		const v3::Scale3& meshscale = mesh.box.scale;
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
		v2::Vec2 ret = v2::unitv / 2 + offset * ((uvCoord - v2::unitv / 2) * -2);
		return ret;
	}

	// Indices of unique vertices for each face of the cube


	// Offset indices for the vertices in each face


	void emit_face(size_t face, const blockmesh& torender, renderer::MeshData& mesh) {
		if (torender.faces[face].uncovered()) {
			const int baselocation = mesh.length();
			const int* uniqueInds = &math::cube_mesh_face_indices[4 * face];
			Scale3 scale = torender.scale();
			Point3 position = torender.center();
			const int textureNumber = torender[face].tex;
			//note: fix was const float lightValue = torender.mesh.invisible() ? torender.lightval.unwrap_or(1) : torender.mesh[face].light;
			//removed it for fixing
			const float lightValue = torender[face].get_light();
			for (size_t j = 0; j < 4; j++) {
				int uniqueind = uniqueInds[j];
				Vec3 offset_from_center = (math::cube_mesh[uniqueind] - unitv / 2) * scale * 2;
				Point3 offset = position + offset_from_center;

				v2::Vec2 uv_coords = face_to_uv_coords(torender, face, j);

				mesh.add_point(offset, uv_coords, textureNumber, lightValue);
			}
			mesh.add_indices_offset_from(math::square_mesh_triangle_indices, baselocation);
		}

	}

	// Emit the faces for a block
	void emit_block(blockmesh& torender, renderer::MeshData& mesh) {
		if (!torender.invisible()) {
			for (int i = 0; i < 6; i++) {
				emit_face(i, torender, mesh);
			}
		}
	}

	// Recreate the mesh for a chunk
	void recreate_chunk_mesh(ecs::obj chunk_to_fill, std::mutex& fill_lock) {

		Chunk::chunk& cnk = chunk_to_fill.get_component<Chunk::chunk>();
		Chunk::chunkmesh& cnk_mesh = chunk_to_fill.get_component<Chunk::chunkmesh>();
		cnk_mesh.recreate_mesh.clean([&]() {
			cnk_mesh.faces.clear();
			renderer::MeshData mesh = cnk_mesh.SolidGeo.create_mesh();
			for (int ind = 0; ind < Chunk::chunksize; ind++) {
				blockmesh& mesh_at = (cnk.block_list[ind].get_component_unchecked<block>().mesh);//g
				if (!mesh_at.is_transparent()) {
					emit_block(mesh_at, mesh);
				}
				else {
					if (!mesh_at.invisible()) {
						for (int x = 0; x < 6; x++) {
							if (mesh_at[x].uncovered()) {
								cnk_mesh.faces.push(mesh_at[x]);
							}
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
	void renderchunk(Chunk::chunkmesh& mesh) {

		mesh.SolidGeo.set_order_key(dist(camera::GetCam().position, mesh.center()));
		mesh.sort_faces();
		renderer::MeshData mesh_data = mesh.TransparentGeo.create_mesh();
		for (int i = 0; i < mesh.faces.length(); i++) {
			emit_face(mesh.faces[i].face_direction.index(), *(mesh.faces[i].mesh), mesh_data);
		}
		mesh.TransparentGeo.set_order_key(dist(camera::GetCam().position, mesh.center()));
		mesh.TransparentGeo.fill(std::move(mesh_data));
	}

	struct ChunkPreMesher :ecs::System {
		ChunkPreMesher() {

		}

		void run(ecs::Ecs& ecs) {
			grid::Grid& world_grid = ecs.get_resource<grid::Grid>().unwrap();
			std::mutex fill_mutex;
			auto recompute_for = [&fill_mutex](Chunk::chunk* item) {
				if (item) {
					recreate_chunk_mesh(item->owner(), fill_mutex);
				}
				};
			thread_util::par_iter(world_grid.chunklist.begin(), world_grid.chunklist.end(), recompute_for, 4);
		};
	};

	struct ChunkRenderer :ecs::System {
		ChunkRenderer() {
		}
		void run(ecs::Ecs& ecs) {
			array<ecs::obj> to_render = array<ecs::obj>();
			Grid& grid = ecs.get_resource<grid::Grid>().unwrap();
			for (int i = 0; i < grid.totalChunks; i++) {
				if (grid[i] && chunk_viewable(*grid[i])) {
					renderchunk(grid[i]->owner().get_component<Chunk::chunkmesh>());
				}
			}

		}
	};
	struct BlockRenderPlugin :Core::Plugin {

		void build(Core::App& engine) {
			engine.emplace_system<grid::GridManager>();
			engine.emplace_system<grid::GridCoverer>();
			engine.emplace_system<grid::GridDarkener>();
			engine.emplace_system<grid::GridLighter>();
			engine.emplace_system<ChunkPreMesher>();
			engine.emplace_system<ChunkRenderer>();
			initblockrendering(engine.Ecs);
		}
		void initblockrendering(ecs::Ecs& ecs) {

			auto& renderer = ecs.get_resource<renderer::Renderer>().unwrap();
			renderer::shader_id block_shader = ecs.load_asset_emplaced<renderer::shader_descriptor>("BlockShader", "shaders\\vert1.vs", "shaders\\frag1.vs").unwrap();
			ecs.load_asset_emplaced<renderer::MaterialDescriptor>("SolidBlock", "solid_phase", "BlockShader", renderer::RenderProperties(true, true, false, false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
				stn::array{
				renderer::uparam("aspect_ratio", "aspectratio"),
				renderer::uparam("proj_matrix", "projection"),
				renderer::uparam("view_matrix", "view"),
				renderer::uparam("bind_block_texture", "tex")
				});

			ecs.load_asset_emplaced<renderer::MaterialDescriptor>("TransparentBlock", "transparent_phase", "BlockShader", renderer::RenderProperties(true, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
				stn::array{ renderer::uparam("aspect_ratio", "aspectratio"),
				renderer::uparam("proj_matrix", "projection"),
				renderer::uparam("view_matrix", "view"),
				renderer::uparam("bind_block_texture", "tex")
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
			texlist.reach(craftingtabletop) = "images\\craftingtabletop.png";
			texlist.reach(craftingtableside) = "images\\craftingtableside.png";
			texlist.reach(crystaltorchtex) = "images\\crystaltorch.png";
			texlist.reach(crystaltorchtoptex) = "images\\crystaltorchtop.png";
			texlist.reach(mosstex) = "images\\moss.png";
			texlist.reach(ropetex) = "images\\rope.png";
			texlist.reach(lavatex) = "images\\lava.png";
			texlist.reach(obsidiantex) = "images\\obb.png";
			texlist.reach(chestfront) = "images\\chest.png";
			texlist.reach(chestside) = "images\\chestsides.png";
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
			renderer.Bind_Texture(texarray);
			renderer.set_uniform("bind_block_texture", texarray);
		}
	};
}
