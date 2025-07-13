
#include "blockrender.h"
#include "../util/geometry.h"
#include "../util/intersection.h"
#include "../game/GameContext.h"
bool blockrender::enablelighting;
Cont::array<float> databuffer;
Cont::array<unsigned int> indicebuffer;

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
TextureArray* blockrender::texarray;
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
void emitface(const int face, block& torender, array<float>& datbuf, array<unsigned int>& indbuf) {
		if (!torender.mesh.faces[face].covered) {
			const int baselocation = datbuf.length / 7;
			const int* uniqueInds = &uniqueindices[4 * face];
			const Vec3& scale = torender.mesh.box.scale;
			const Vec3& position = torender.mesh.box.center;

			// Precompute texture number and lighting
			const int textureNumber = torender.mesh[face].tex;
			const float lightValue = blockrender::enablelighting ?
				(torender.attributes.transparent ? torender.lightval : torender.mesh[face].light) : 15;

			float vertexData[28]{}; // 4 vertices * 7 floats per vertex (x, y, z, u, v, tex, light)
			unsigned int indices[6]; // 6 indices per face

			for (int j = 0; j < 4; j++) {
				int uniqueind = uniqueInds[j];
				Vec3 offsetfromcenter = (vert[uniqueind] - unitv / 2) * scale * 2;
				Vec3 offset = position + offsetfromcenter;

				// Calculate UV coordinates
				v2::Vec2 coords = facecoordtouv(&torender[face], j);

				// Fill vertex data
				int baseIndex = j * 7;
				vertexData[baseIndex] = offset.x;
				vertexData[baseIndex + 1] = offset.y;
				vertexData[baseIndex + 2] = offset.z;
				vertexData[baseIndex + 3] = coords.x;
				vertexData[baseIndex + 4] = coords.y;
				vertexData[baseIndex + 5] = textureNumber;
				vertexData[baseIndex + 6] = lightValue;
			}

			// Append vertex data in bulk
			datbuf.push(vertexData, 28);

			// Generate and append indices
			for (int j = 0; j < 6; j++) {
				indices[j] = baselocation + indiceoffsetfrombaselocation[j];
			}
			indbuf.push(indices, 6);
		}
	
}

// Emit the faces for a block
void emitblock(block& torender, array<float>& datbuf, array<unsigned int>& indbuf) {
	if (torender.id != minecraftair) {
		for (int i = 0; i < 6; i++) {
			emitface(i, torender, datbuf, indbuf);
		}
	}
}

// Recreate the mesh for a chunk
void recreatechunkmesh(Chunk::chunk* aschunk) {
	
	aschunk->mesh->facebuf.destroy();
	aschunk->mesh->facebuf = Cont::array<face>();
	Cont::array<unsigned int> indbuf = Cont::array<unsigned int>();
	Cont::array<float> datbuf= Cont::array<float>();

	
	for (int ind = 0; ind < chunksize; ind++) {
		block& blockatpos = (aschunk->blockbuf[ind].getcomponent<block>());
		
		if (!blockatpos.attributes.transparent) {
			emitblock(blockatpos, datbuf, indbuf);
			continue;
		}
		
		if (blockatpos.id == minecraftair) {
			
				continue;
		}


		for (int x = 0; x < 6; x++) {
					if (!(blockatpos.mesh)[x].covered) {
						aschunk->mesh->facebuf.push((blockatpos.mesh)[x]);
					}
		}
		
	}
	CtxName::ctx.Ren->Fill(&aschunk->mesh->SolidGeo, datbuf, indbuf);
	aschunk->mesh->meshsize = indbuf.length;
	datbuf.destroy();
	indbuf.destroy();
	
}

// Render a chunk mesh
void renderchunk(Chunk::chunkmesh& mesh, bool transparent) {
	if (!transparent) {
		CtxName::ctx.Ren->Render(&mesh.SolidGeo);
	}
	else {
		// Enable 2D render
		array<float> datbuf = array<float>();
		array<unsigned int> indbuf = array<unsigned int>();

		for (int i = 0; i < mesh.facebuf.length; i++) {
			emitface(mesh.facebuf[i].facenum.ind(), *(mesh.facebuf[i].mesh->blk), datbuf, indbuf);
		}

		CtxName::ctx.Ren->Render(&mesh.TransparentGeo, datbuf, indbuf);
		datbuf.destroy();
		indbuf.destroy();
	}
}
void blockrender::setrendertransparent()
{

	CtxName::ctx.Ren->SetType("TransparentBlock");
	
}

void ApplyBlockTex(renderer::Renderer* rend) {

rend->context.Bind(*blockrender::texarray);
}
void blockrender::setrendersolid()
{
	CtxName::ctx.Ren->SetType("SolidBlock");

	
}

// Initialize the data buffer and render chunks
void blockrender::renderblocks(bool rendertransparent) {
	CtxName::ctx.Ren->context.Bind(CtxName::ctx.Ren->Shaders["BlockShader"]);

	array<Chunk::chunk> tosort = array<Chunk::chunk>();

	for (int i = 0; i < CtxName::ctx.Grid->totalChunks; i++) {
		
		if (chunkviewable(CtxName::ctx.GridRef()[i])) {
			if (CtxName::ctx.GridRef()[i]->mesh->meshrecreateneeded) {
				recreatechunkmesh(CtxName::ctx.GridRef()[i]);
				CtxName::ctx.GridRef()[i]->mesh->meshrecreateneeded = false;
			}

			CtxName::ctx.GridRef()[i]->mesh->sortbuf();

			tosort.push(* (CtxName::ctx.GridRef()[i]));
		}
	}

	oalgorithm::quicksort<Chunk::chunk>(tosort.list, tosort.length);
	setrendersolid();
	int renderamt = 0;
		for (int i = 0; i < tosort.length; i++) {
		
				renderamt++;
				renderchunk(*tosort[i].mesh, false);
			
		}
 		
	

		setrendertransparent();
		for (int i = 0; i < tosort.length; i++) {
		
				renderchunk(*tosort[i].mesh, true);
			
		}
	
	tosort.destroy();
}

void blockrender::initblockrendering()
{
	CtxName::ctx.Ren->Shaders.Compile( "BlockShader","shaders\\vert1.vs", "shaders\\frag1.vs");
	CtxName::ctx.Ren->AddType(Base_Material("SolidBlock", "BlockShader", RenderProperties(true, true, false, false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA))
	.AddUniform(renderer::setAspectRatio).AddUniform(renderer::setrenderingmatrixes).AddUniform(ApplyBlockTex));

	CtxName::ctx.Ren->AddType(Base_Material("TransparentBlock", "BlockShader", RenderProperties(true, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)).
	AddUniform(renderer::setAspectRatio).AddUniform(renderer::setrenderingmatrixes).AddUniform(ApplyBlockTex));

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
	texarray = CtxName::ctx.Ren->Textures.GetTexArray(texlist,"BlockTextures");
	CtxName::ctx.Ren->context.Bind(*texarray);
	enablelighting = true;
}

