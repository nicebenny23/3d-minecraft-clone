#include "../util/thread_split.h"
#include "blockrender.h"
#include "../util/geometry.h"
#include "../util/intersection.h"

#include "../game/GameContext.h"
#include <mutex>
bool blockrender::enablelighting;
stn::array<float> databuffer;
stn::array<unsigned int> indicebuffer;

using namespace grid;

// Predefined vertices for a cube
const Vec3 vert[] = {
	Vec3(0, 0, 0), // vertex 0
	Vec3(1, 0, 0), // vertex 1
	Vec3(1, 1, 0), // vertex 2
	Vec3(0, 1, 0), // vertex 3
	Vec3(0, 0, 1), // vertex 4
	Vec3(1, 0, 1), // vertex 5
	Vec3(1, 1, 1), // vertex 6
	Vec3(0, 1, 1)  // vertex 7
};

// UV coordinates for cube mapping
const float cubeuv[] = {
	1, 0,
	0, 0,
	0, 1,
	1, 1
};
// Check if a chunk is viewable within the camera's frustum
bool chunkviewable(Chunk::chunk* chk) {
	return true;
	float slope = tan(CtxName::ctx.Ren->fov / 2);
	geometry::Box chkb = geometry::Box(chk->center(), unitv * float( chunklength )/ 2.f);
	ray camray = ray(camera::campos(), camera::campos() + camera::GetCamFront() * 1);
	geometry::cone ncone = geometry::cone(camray, slope);
	geometry::Plane pln = geometry::Plane(camera::GetCamFront(), camray.start);
	bool srf = false;
	for (int i = 0; i < 8; i++)
	{
		Vec3 vertex = chk->center() + (vert[i] - unitv / 2.f) *float( chunklength);
		if (dot(vertex - camera::campos(), camera::GetCamFront()) > 0) {

			srf = true;
		}
	}
	//return true;
	if (!srf)
	{
		return false;
	}
	return geointersect::intersects(ncone, geometry::sphere(chkb));

}

// Calculate UV coordinates for a face centered at the mesh
v2::Vec2 facecoordtouv(const face* fce, int ind) {
	const v3::Vec3& meshscale = fce->mesh->box.scale;
	int facetype = fce->facenum.ind() / 2;
	v2::Vec2 offset;

	switch (facetype) {
	case 0:
		offset = v2::Vec2(meshscale.z, meshscale.y)/blocksize;
		break;
	case 1:
		offset = v2::Vec2(meshscale.x, meshscale.z)/blocksize;
		break;
	case 2:
		offset = v2::Vec2(meshscale.x, meshscale.y)/blocksize;
		break;
	default:
		Assert("invalid direction");
		break;
	}

	v2::Vec2 uvCoord = v2::Vec2(cubeuv[2 * ind], cubeuv[2 * ind + 1]);
	v2::Vec2 ret = v2::unitv / 2 + offset * ((uvCoord - v2::unitv / 2) * -2);
	return ret;
}

// Indices of unique vertices for each face of the cube
const int uniqueindices[] = {
	5, 1, 2, 6,  // east (+x)
	0, 4, 7, 3,  // west (-x)
	2, 3, 7, 6,  // top (+y)
	0, 1, 5, 4,  // bottom (-y)
	4, 5, 6, 7,  // south (+z)
	0, 1, 2, 3   // north (-z)
};

// Offset indices for the vertices in each face
const int indiceoffsetfrombaselocation[] = {
	0, 1, 2, 0, 2, 3
};

// Emit the vertices and indices for a single face of a block
void emitface(const int face, block& torender, renderer::MeshData& mesh) {
		if (torender.mesh.faces[face].cover==cover_state::Uncovered) {
			const int baselocation = mesh.length();
			const int* uniqueInds = &uniqueindices[4 * face];
			const Vec3& scale = torender.mesh.box.scale;
			const Vec3& position = torender.mesh.box.center;

			// Precompute texture number and lighting
			const int textureNumber = torender.mesh[face].tex;
			const float lightValue = blockrender::enablelighting ?
				(torender.attributes.transparent ? torender.lightval : torender.mesh[face].light) : 15;

		
			for (int j = 0; j < 4; j++) {
				int uniqueind = uniqueInds[j];
				Vec3 offsetfromcenter = (vert[uniqueind] - unitv / 2) * scale * 2;
				Vec3 offset = position + offsetfromcenter;

				// Calculate UV coordinates
				v2::Vec2 coords = facecoordtouv(&torender[face], j);

				// Fill vertex data
				mesh.add_point(offset,coords,textureNumber,lightValue);
				//mesh.pointlist.push(offset.x, offset.y, offset.z, coords.x, coords.y, textureNumber, lightValue);

			}

			
			// Generate and append indices
			for (int j = 0; j < 6; j++) {
				mesh.add_index(baselocation + indiceoffsetfrombaselocation[j]);
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
void recreatechunkmesh(Chunk::chunk* aschunk,std::mutex& fill_lock) {
	
	aschunk->mesh->facebuf.destroy();//g
	aschunk->mesh->facebuf = stn::array<face>();//g
	renderer::MeshData mesh= aschunk->mesh->SolidGeo.create_mesh();
	
	for (int ind = 0; ind < chunksize; ind++) {
		block& blockatpos = (aschunk->blockbuf[ind].getcomponent<block>());//g
		
		if (!blockatpos.attributes.transparent) {
			emitblock(blockatpos, mesh);//g
			continue;
		}
		
		if (blockatpos.id == minecraftair) {
			
				continue;
		}


		for (int x = 0; x < 6; x++) {
					if ((blockatpos.mesh)[x].cover==cover_state::Uncovered) {
						aschunk->mesh->facebuf.push((blockatpos.mesh)[x]);
					}
		}
		
	}
	
	{
		std::unique_lock lck(fill_lock);

		aschunk->mesh->SolidGeo.fill(std::move(mesh));
	}

	aschunk->mesh->meshrecreateneeded = false;
}

// Render a chunk mesh
void renderchunk(Chunk::chunkmesh& mesh, bool transparent) {
	if (!transparent) {
		mesh.SolidGeo.render();
	}
	else {
		// Enable 2D render
		renderer::MeshData mesh_data=mesh.TransparentGeo.create_mesh();
		for (int i = 0; i < mesh.facebuf.length; i++) {
			emitface(mesh.facebuf[i].facenum.ind(), *(mesh.facebuf[i].mesh->blk), mesh_data);
		}

		mesh.TransparentGeo.fill(std::move(mesh_data));
		CtxName::ctx.Ren->consume();
		mesh.TransparentGeo.render();
	
	}
}


// Initialize the data buffer and render chunks
void blockrender::renderblocks(bool rendertransparent) {
	//CtxName::ctx.Ren->context.Bind(CtxName::ctx.Ren->Shaders["BlockShader"]);

	array<Chunk::chunk*> tosort = array<Chunk::chunk*>();

	for (int i = 0; i < CtxName::ctx.Grid->totalChunks; i++) {
		if (CtxName::ctx.GridRef()[i]==nullptr)
		{
			continue;
		}
		if (chunkviewable(CtxName::ctx.GridRef()[i])) {
			tosort.push((CtxName::ctx.GridRef()[i]));
		}
	}
	std::mutex fill_mutex;

	auto func = [&fill_mutex](Chunk::chunk*& item) {
		
		if (item->mesh->meshrecreateneeded) {
			recreatechunkmesh(item, fill_mutex);  // or just recreatechunkmesh(item);
		}
		item->mesh->sortbuf();
		};
	
	thread_util::par_iter(tosort.begin(), tosort.end(), func, 4);
	CtxName::ctx.Ren->consume();
	
	if (tosort.length != 0)
	{

		std::stable_sort(tosort.begin(), tosort.end(), [](Chunk::chunk* a, Chunk::chunk* b) {
			return (*a) < (*b);
			});
	}	
		int renderamt = 0;
		for (int i = 0; i < tosort.length; i++) {
		
				renderamt++;
				renderchunk(*tosort[i]->mesh, false);
			
		}
 		
	
	
		for (int i = 0; i < tosort.length; i++) {
		
				renderchunk(*tosort[i]->mesh, true);
			
		}
	
	tosort.destroy();
}
struct bind_block_texture {};
void blockrender::initblockrendering()
{
	auto* renderer = CtxName::ctx.Ren;
	CtxName::ctx.Ren->Shaders.Compile( "BlockShader","shaders\\vert1.vs", "shaders\\frag1.vs");
	renderer->Construct("SolidBlock", "BlockShader", RenderProperties(true, true, false, false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
		uniforms::uparam("aspect_ratio","aspectratio"),
		uniforms::uparam("proj_matrix","projection"),
		uniforms::uparam("view_matrix","view"),
		uniforms::uparam("bind_block_texture","tex")
	);
	
	renderer->Construct("TransparentBlock", "BlockShader", RenderProperties(true, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
		uniforms::uparam("aspect_ratio", "aspectratio"),
		uniforms::uparam("proj_matrix", "projection"),
		uniforms::uparam("view_matrix", "view"),
		uniforms::uparam("bind_block_texture", "tex")
	);

	array<const char*> texlist = array<const char*>();
	texlist.reach(treestonetex)= "images\\treestone.png";
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
	
	Ids::Id texarray = CtxName::ctx.Ren->Textures.LoadTextureArray(texlist,"BlockTextures");
	
	CtxName::ctx.Ren->Bind_Texture(texarray);
	CtxName::ctx.Ren->set_uniform("bind_block_texture",texarray);
	enablelighting = true;
}

