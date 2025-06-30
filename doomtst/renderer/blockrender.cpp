
#include "blockrender.h"
#include "../util/geometry.h"
#include "../util/intersection.h"
#include "../game/GameContext.h"
bool blockrender::enablelighting;
Cont::array<float> databuffer;
Cont::array<unsigned int> indicebuffer;

using namespace grid;

// Predefined vertices for a cube
const Vector3 vert[] = {
	Vector3(0, 0, 0), // vertex 0
	Vector3(1, 0, 0), // vertex 1
	Vector3(1, 1, 0), // vertex 2
	Vector3(0, 1, 0), // vertex 3
	Vector3(0, 0, 1), // vertex 4
	Vector3(1, 0, 1), // vertex 5
	Vector3(1, 1, 1), // vertex 6
	Vector3(0, 1, 1)  // vertex 7
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
	float slope = tan(renderer::fov / 2);
	geometry::Box chkb = geometry::Box(chk->center(), unitv * float( chunklength )/ 2.f);
	ray camray = ray(camera::campos(), camera::campos() + camera::GetCamFront() * 1);
	geometry::cone ncone = geometry::cone(camray, slope);
	geometry::Plane pln = geometry::Plane(camera::GetCamFront(), camray.start);
	bool srf = false;
	for (int i = 0; i < 8; i++)
	{
		Vector3 vertex = chk->center() + (vert[i] - unitv / 2.f) *float( chunklength);
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
v2::Vector2 facecoordtouv(const face* fce, int ind) {
	const v3::Vector3& meshscale = fce->mesh->box.scale;
	int facetype = fce->facenum.ind() / 2;
	v2::Vector2 offset;

	switch (facetype) {
	case 0:
		offset = v2::Vector2(meshscale.z, meshscale.y)/blocksize;
		break;
	case 1:
		offset = v2::Vector2(meshscale.x, meshscale.z)/blocksize;
		break;
	case 2:
		offset = v2::Vector2(meshscale.x, meshscale.y)/blocksize;
		break;
	default:
		Assert("invalid direction");
		break;
	}

	v2::Vector2 uvCoord = v2::Vector2(cubeuv[2 * ind], cubeuv[2 * ind + 1]);
	v2::Vector2 ret = v2::unitv / 2 + offset * ((uvCoord - v2::unitv / 2) * -2);
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
			const Vector3& scale = torender.mesh.box.scale;
			const Vector3& position = torender.mesh.box.center;

			// Precompute texture number and lighting
			const int textureNumber = torender.mesh[face].tex;
			const float lightValue = blockrender::enablelighting ?
				(torender.attributes.transparent ? torender.lightval : torender.mesh[face].light) : 15;

			float vertexData[28]{}; // 4 vertices * 7 floats per vertex (x, y, z, u, v, tex, light)
			unsigned int indices[6]; // 6 indices per face

			for (int j = 0; j < 4; j++) {
				int uniqueind = uniqueInds[j];
				Vector3 offsetfromcenter = (vert[uniqueind] - unitv / 2) * scale * 2;
				Vector3 offset = position + offsetfromcenter;

				// Calculate UV coordinates
				v2::Vector2 coords = facecoordtouv(&torender[face], j);

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
	Cont::array<float> datbuf= Cont::array<float>(0,false);

	
	for (int ind = 0; ind < chunksize; ind++) {
		block& blockatpos = (aschunk->blockbuf[ind]);
		
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
	renderer::Ren.Fill(&aschunk->mesh->SolidGeo, datbuf, indbuf);
	aschunk->mesh->meshsize = indbuf.length;
	datbuf.destroy();
	indbuf.destroy();
	
}

// Render a chunk mesh
void renderchunk(Chunk::chunkmesh& mesh, bool transparent) {
	if (!transparent) {
		renderer::Ren.Render(&mesh.SolidGeo);
	}
	else {
		// Enable 2D render
		array<float> datbuf = array<float>();
		array<unsigned int> indbuf = array<unsigned int>();

		for (int i = 0; i < mesh.facebuf.length; i++) {
			emitface(mesh.facebuf[i].facenum.ind(), *(mesh.facebuf[i].mesh->blk), datbuf, indbuf);
		}

		renderer::Ren.Render(&mesh.TransparentGeo, datbuf, indbuf);
		datbuf.destroy();
		indbuf.destroy();
	}
}
void blockrender::setrendertransparent()
{

	renderer::Ren.SetType("TransparentBlock");
	renderer::Ren.Textures.Apply(blockrender::texarray);
	renderer::setrenderingmatrixes();
	renderer::setaspectratio();
}

void blockrender::setrendersolid()
{
	renderer::Ren.SetType("SolidBlock");
	renderer::Ren.Textures.Apply(blockrender::texarray);
	renderer::setrenderingmatrixes();
	renderer::setaspectratio();
	
}

// Initialize the data buffer and render chunks
void blockrender::renderblocks(bool rendertransparent) {
	renderer::Ren.Shaders.Bind("BlockShader");

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
	renderer::Ren.Shaders.Compile( "BlockShader","shaders\\vert1.vs", "shaders\\frag1.vs");
	renderer::Ren.AddType(RenderMode("SolidBlock", "BlockShader", RenderProperties(true, true, false, false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)));

	renderer::Ren.AddType(RenderMode("TransparentBlock", "BlockShader", RenderProperties(true, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)));
	array<const char*> texlist = array<const char*>();
	texlist[treestonetex] = "images\\treestone.png";
	texlist[grasstex] = "images\\grass.png";
	texlist[stonetex] = "images\\stone.png";
	texlist[altartex] = "images\\crystalaltarside.png";
	texlist[glasstex] = "images\\glass.png";
	texlist[watertex] = "images\\water.png";
	texlist[torchtex] = "images\\torch.png";
	texlist[torchtoptex] = "images\\torchtop.png";
	texlist[crystaloretex] = "images\\crystalore.png";
	texlist[craftingtabletop] = "images\\craftingtabletop.png";
	texlist[craftingtableside] = "images\\craftingtableside.png";
	texlist[crystaltorchtex] = "images\\crystaltorch.png";
	texlist[crystaltorchtoptex] = "images\\crystaltorchtop.png";
	texlist[mosstex] = "images\\moss.png";
	texlist[ropetex] = "images\\rope.png";
	texlist[lavatex] = "images\\lava.png";
	texlist[obsidiantex] = "images\\obb.png";
	texlist[chestfront] = "images\\chest.png";

	texlist[chestside] = "images\\chestsides.png";
	texlist[furnacefront] = "images\\furnacetop.png";

	texlist[furnaceside] = "images\\furnace.png";
	texlist[ironoretex] = "images\\ironore.png";

	texlist[furnacesideon] = "images\\furnaceon.png";
	texlist[furnacefronton] = "images\\furnacetopon.png";
	texlist[logtoppng] = "images\\log.png";
	texlist[ultraaltarpngultrapng] = "images\\ultraaltar.png";
	texlist[sandtex] = "images\\sand.png";
	texlist[planktex] = "images\\treestoneblock.png";
	texarray = renderer::Ren.Textures.GetTexArray(texlist,"BlockTextures");
	renderer::Ren.Textures.Apply(texarray);
	enablelighting = true;
}

