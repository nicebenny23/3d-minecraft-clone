#include "blockrender.h"
#include "../util/geometry.h"
bool blockrender::enablelighting;
dynamicarray::array<float> databuffer;
dynamicarray::array<unsigned int> indicebuffer;

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
texturearray blockrender::texarray;
// Check if a chunk is viewable within the camera's frustum
bool chunkviewable(Chunk::chunk* chk) {
	float slope = tan(renderer::fov / 2);
	geometry::Box chkb = geometry::Box(chk->center(), unitv * 8);
	ray camray = ray(camera::campos, v3::Vector3(camera::campos) + camera::frontvec * 1);
	geometry::cone ncone = geometry::cone(camray, slope);
	geometry::Plane pln = geometry::Plane(camera::frontvec, camray.start);
	bool srf = false;
	for (int i = 0; i < 8; i++)
	{
		Vector3 vertex = chk->center() + (vert[i] - unitv / 2.f) * 16;
		if (dotproduct(vertex - camera::campos, camera::frontvec) > 0) {

			srf = true;
		}
	}
	if (!srf)
	{
		return false;
	}
	return ncone.intersectssphere(geometry::sphere(chkb));
}

// Calculate UV coordinates for a face centered at the mesh
v2::Vector2 facecoordtouv(const face* fce, int ind) {
	const v3::Vector3& meshscale = fce->mesh->scale;
	int facetype = fce->facenum / 2;
	v2::Vector2 offset;

	switch (facetype) {
	case 0:
		offset = v2::Vector2(meshscale.z, meshscale.y);
		break;
	case 1:
		offset = v2::Vector2(meshscale.x, meshscale.z);
		break;
	case 2:
		offset = v2::Vector2(meshscale.x, meshscale.y);
		break;
	default:
		// Handle unexpected facetype values if necessary
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
void emitface(int face, block& torender, array<float>& datbuf, array<unsigned int>& indbuf) {
	if (!torender[face].covered) {
		const int baselocation = datbuf.length / 7;
		for (int j = 0; j < 4; j++) {
			// Index of unique vertex in each face
			int uniqueind = uniqueindices[4 * face + j];
			// Actual location
			Vector3 offsetfromcenter = (vert[uniqueind] - unitv / 2) * Vector3(torender.mesh.scale) * 2;
			Vector3 offset = torender.mesh.pos + offsetfromcenter;
			datbuf.append(offset.x);
			datbuf.append(offset.y);
			datbuf.append(offset.z);

			// UV coordinates
			v2::Vector2 coords = facecoordtouv(&torender[face], j);
			datbuf.append(coords.x);
			datbuf.append(coords.y);

			// Texture number
			int texturenumb = ((torender).mesh)[face].tex;
			datbuf.append(texturenumb);

			// Light value
			if (blockrender::enablelighting)
			{

				if (torender.transparent) {

					datbuf.append(torender.lightval);
				}
				else {
					datbuf.append((torender.mesh)[face].light);
				}
			}
			else
			{
				databuffer.append(15);
			}
		}

		for (int j = 0; j < 6; j++) {
			int indicelocation = baselocation + indiceoffsetfrombaselocation[j];
			indbuf.append(indicelocation);
		}
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
	aschunk->mesh->indbuf.destroy();
	aschunk->mesh->datbuf.destroy();
	aschunk->mesh->facebuf.destroy();
	aschunk->mesh->facebuf = dynamicarray::array<face>();
	aschunk->mesh->indbuf = dynamicarray::array<unsigned int>();
	aschunk->mesh->datbuf = dynamicarray::array<float>();

	for (int ind = 0; ind < chunksize; ind++) {
		block& blockatpos = (aschunk->blockbuf[ind]);

		if (!blockatpos.transparent) {
			emitblock(blockatpos, aschunk->mesh->datbuf, aschunk->mesh->indbuf);
			continue;
		}
		
		if (blockatpos.id == minecraftair) {
			
				continue;
		}


		for (int x = 0; x < 6; x++) {
					if (!(blockatpos.mesh)[x].covered) {
						aschunk->mesh->facebuf.append((blockatpos.mesh)[x]);
					}
		}
			
		
	}
}

// Render a chunk mesh
void renderchunk(Chunk::chunkmesh& mesh, bool transparent) {
	if (!transparent) {
		renderer::renderquadlist(mesh.Voa, mesh.ibo, mesh.VBO, mesh.datbuf, mesh.indbuf);
	}
	else {
		// Enable 2D render
		array<float> datbuf = array<float>();
		array<unsigned int> indbuf = array<unsigned int>();

		for (int i = 0; i < mesh.facebuf.length; i++) {
			emitface(mesh.facebuf[i].facenum, *(mesh.facebuf[i].mesh->blk), datbuf, indbuf);
		}
		renderer::renderquadlist(mesh.Voa, mesh.ibo, mesh.VBO, datbuf, indbuf);
		datbuf.destroy();
		indbuf.destroy();
	}
}

void blockrender::setrendertransparent()
{
	glDepthFunc(GL_LESS);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderer::currshader = renderer::normalshader;
	renderer::shaderlist[renderer::normalshader].attach();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	texarray.apply();
	renderer::setrenderingmatrixes();

}

void blockrender::setrendersolid()
{
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);
	renderer::currshader = renderer::normalshader;
	renderer:: shaderlist[renderer::normalshader].attach();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	texarray.apply();
	renderer::setrenderingmatrixes();
}

// Initialize the data buffer and render chunks
void blockrender::renderblocks(bool rendertransparent) {
	glUseProgram(renderer::shaderlist[renderer::normalshader].id);
	for (int i = 0; i < totalgridsize; i++) {
		if (chunklist[i]->mesh->meshrecreateneeded) {
			recreatechunkmesh(chunklist[i]);
			chunklist[i]->mesh->meshrecreateneeded = false;
		}
		chunklist[i]->mesh->sortbuf();
	}

	array<Chunk::chunk> tosort = array<Chunk::chunk>();

	for (int i = 0; i < totalgridsize; i++) {
		tosort.append(*(chunklist[i]));
	}

	oalgorithm::quicksort<Chunk::chunk>(tosort.getdata(), tosort.length);
	setrendersolid();
	int renderamt = 0;
		for (int i = 0; i < totalgridsize; i++) {
			if (chunkviewable(&tosort[i])) {
				renderamt++;
				renderchunk(*tosort[i].mesh, false);
			}
		}
 		int l = 1;
	

		setrendertransparent();
		for (int i = 0; i < totalgridsize; i++) {
			if (chunkviewable(&tosort[i])) {
				renderchunk(*tosort[i].mesh, true);
			}
		}
	
	tosort.destroy();
}

void blockrender::initblockrendering()
{
	renderer::shaderlist[renderer::normalshader] = shader::shader("shaders\\vert1.vs", "shaders\\frag1.vs");
	renderer::shaderlist[renderer::normalshader].attach();
	renderer::currshader = renderer::normalshader;
	array<const char*> texlist = array<const char*>();
	texlist[0] = "images\\treestone.png";
	texlist[1] = "images\\grass.png";
	texlist[2] = "images\\stone.png";
	texlist[3] = "images\\stone.png";
	texlist[4] = "images\\glass.png";
	texlist[5] = "images\\water.png";
	texlist[6] = "images\\torch.png";
	texlist[7] = "images\\torchtop.png";
	texlist[8] = "images\\crystalore.png";
	texlist[9] = "images\\craftingtabletop.png";
	texlist[10] = "images\\craftingtableside.png";
	texlist[11] = "images\\crystaltorch.png";
	texlist[12] = "images\\crystaltorchtop.png";
	texlist[13] = "images\\moss.png";
	texlist[14] = "images\\rope.png";
	texlist[15] = "images\\lava.png";
	texlist[16] = "images\\obb.png";
	texlist[chestfront] = "images\\chest.png";

	texlist[chestside] = "images\\chestsides.png";
	texlist[furnacefront] = "images\\furnacetop.png";

	texlist[furnaceside] = "images\\furnace.png";
	texlist[ironoretex] = "images\\ironore.png";

	texlist[furnacesideon] = "images\\furnaceon.png";
	texlist[furnacefronton] = "images\\furnacetopon.png";
	texlist[logtoppng] = "images\\log.png";
	texarray = texturearray(16, 16, texlist);
	texarray.apply();
	enablelighting = true;
}

