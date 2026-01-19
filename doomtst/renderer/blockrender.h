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

	

	bool chunkviewable(Chunk::chunk& chk) {
		float slope = tan(CtxName::ctx.Ren->fov / 2);
		geo::Box chkb = geo::Box(chk.center(), unit_scale * float(chunklength) / 2.f);
		ray camray = ray::from_offset(camera::campos(), camera::GetCamFront());
		geo::cone ncone = geo::cone(camray, slope);
		geo::Plane pln = geo::Plane(camera::GetCamFront(), camray.start);
		bool srf = false;
		for (int i = 0; i < 8; i++) {
			Point3 vertex = chk.center() + (math::cube_mesh[i] - unitv / 2.f) * float(chunklength);
			if (dot(vertex - camera::campos(), camera::GetCamFront()) > 0) {

				srf = true;
			}
		}

		if (!srf) {
			return false;
		}
		return geointersect::intersects(ncone, geo::Sphere(chkb));

	}

	// Calculate UV coordinates for a face centered at the mesh
	v2::Vec2 facecoordtouv(const face* fce, int ind) {
		const v3::Scale3& meshscale = fce->mesh->box.scale;
		int facetype = fce->facenum.ind() / 2;
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
		Assert("invalid direction");
		break;
		}

		v2::Vec2 uvCoord = math::square_mesh[ind] ;
		v2::Vec2 ret = v2::unitv / 2 + offset * ((uvCoord - v2::unitv / 2) * -2);
		return ret;
	}

	// Indices of unique vertices for each face of the cube


	// Offset indices for the vertices in each face
	

	void emitface(int face, const block& torender, renderer::MeshData& mesh) {
		if (torender.mesh.faces[face].cover == cover_state::Uncovered) {
			const int baselocation = mesh.length();
			const int* uniqueInds = &math::cube_mesh_face_indices[4 * face];
			Scale3 scale = torender.scale();
			Point3 position = torender.center();
			const int textureNumber = torender.mesh[face].tex;
			const float lightValue = torender.attributes.transparent ? torender.lightval.unwrap_or(1) : torender.mesh[face].light;
			for (int j = 0; j < 4; j++) {
				int uniqueind = uniqueInds[j];
				Vec3 offsetfromcenter = (math::cube_mesh[uniqueind] - unitv / 2) * scale * 2;
				Point3 offset = position + offsetfromcenter;

				v2::Vec2 coords = facecoordtouv(&torender.mesh[face], j);

				mesh.add_point(offset, coords, textureNumber, lightValue);
			}
			for (int j = 0; j < 6; j++) {
				mesh.add_index(baselocation + math::square_mesh_triangle_indices[j]);
			}
		}

	}

	// Emit the faces for a block
	void emitblock(block& torender, renderer::MeshData& mesh) {
		if (torender.id != minecraftair) {
			for (int i = 0; i < 6; i++) {
				emitface(i, torender, mesh);
			}
		}
	}

	// Recreate the mesh for a chunk
	void recreatechunkmesh(Chunk::chunk& chunk_to_fill, std::mutex& fill_lock) {

		chunk_to_fill.mesh->faces.clear();
		renderer::MeshData mesh = chunk_to_fill.mesh->SolidGeo.create_mesh();
		for (int ind = 0; ind < chunksize; ind++) {
			block& blockatpos = (chunk_to_fill.blockbuf[ind].get_component_unchecked<block>());//g

			if (!blockatpos.attributes.transparent) {
				emitblock(blockatpos, mesh);//g
				continue;
			}
			else {
				if (blockatpos.id == minecraftair) {
					continue;
				}
				for (int x = 0; x < 6; x++) {
					if (blockatpos.mesh[x].uncovered()) {
						chunk_to_fill.mesh->faces.push(blockatpos.mesh[x]);
					}
				}
			}
		}
		{
			std::unique_lock lck(fill_lock);
			chunk_to_fill.mesh->SolidGeo.fill(std::move(mesh));
		}
	}



	// Render a chunk mesh
	void renderchunk(Chunk::chunkmesh& mesh, bool transparent) {
		if (!transparent) {
			mesh.SolidGeo.set_order_key(dist(camera::GetCam().position, mesh.owner.center()));
		}
		else {
			mesh.sort_faces();
			renderer::MeshData mesh_data = mesh.TransparentGeo.create_mesh();
			for (int i = 0; i < mesh.faces.length(); i++) {
				emitface(mesh.faces[i].facenum.ind(), *(mesh.faces[i].mesh->blk), mesh_data);
			}

			mesh.TransparentGeo.set_order_key(dist(camera::GetCam().position, mesh.owner.center()));

			mesh.TransparentGeo.fill(std::move(mesh_data));
		

		}
	}

	struct ChunkPreMesher :ecs::System {
		ChunkPreMesher() {

		}

		void run(ecs::Ecs& ecs) {
			grid::Grid& world_grid = ecs.get_resource<grid::Grid>().unwrap();
			std::mutex fill_mutex;
			auto recompute_for = [&fill_mutex](Chunk::chunk* item) {
				if (item) {

					item->mesh->recreate_mesh.clean([&item, &fill_mutex]() {
						recreatechunkmesh(*item, fill_mutex);
						});
				}
				};

			thread_util::par_iter(world_grid.chunklist.begin(), world_grid.chunklist.end(), recompute_for, 4);
		}
	};
	struct ChunkRenderer :ecs::System {
		ChunkRenderer() {
		}
		void run(ecs::Ecs& ecs) {
			array<Chunk::chunk*> to_render = array<Chunk::chunk*>();
			Grid& grid = ecs.get_resource<grid::Grid>().unwrap();
			for (int i = 0; i < grid.totalChunks; i++) {

				if (grid[i] && chunkviewable(*grid[i])) {
					to_render.push(grid[i]);
				}
			}

			
			if (to_render.length() != 0) {
				std::stable_sort(to_render.begin(), to_render.end(), [](Chunk::chunk* a, Chunk::chunk* b) {
					return dist(b->center(), camera::campos()) < dist(a->center(), camera::campos());
				});
			}
			for (int i = 0; i < to_render.length(); i++) {
				renderchunk(*to_render[i]->mesh, false);
			}

			for (int i = 0; i < to_render.length(); i++) {
				renderchunk(*to_render[i]->mesh, true);
			}
		}
	};
	struct BlockRenderPlugin :Core::Plugin {

		void build(Core::App& engine) {
			engine.emplace_system<gridutil::GridManager>();
			engine.emplace_system<gridutil::GridCoverer>();
			engine.emplace_system<gridutil::GridDarkener>();
			engine.emplace_system<gridutil::GridLighter>();
			engine.emplace_system<ChunkPreMesher>();
			engine.emplace_system<ChunkRenderer>();
			initblockrendering(engine.Ecs);
		}
		void initblockrendering(ecs::Ecs& ecs) {

			auto& renderer = ecs.get_resource<renderer::Renderer>().unwrap();
			renderer::shader_id block_shader=CtxName::ctx.Ecs->load_asset_emplaced<renderer::shader_descriptor>("BlockShader", "shaders\\vert1.vs", "shaders\\frag1.vs").unwrap();
			ecs.load_asset_emplaced<renderer::MaterialDescriptor>("SolidBlock", "solid_phase", "BlockShader", renderer::RenderProperties(true, true, false, false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
				stn::array{
				renderer::uparam("aspect_ratio", "aspectratio"),
				renderer::uparam("proj_matrix", "projection"),
				renderer::uparam("view_matrix", "view"),
				renderer::uparam("bind_block_texture", "tex")
				});

			ecs.load_asset_emplaced<renderer::MaterialDescriptor>("TransparentBlock", "transparent_phase", "BlockShader", renderer::RenderProperties(true, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
				stn::array{renderer::uparam("aspect_ratio", "aspectratio"),
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
