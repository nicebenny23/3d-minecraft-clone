#include "blockrender.h"
#include "../util/vector2.h"
dynamicarray::array<float> databuffer;
dynamicarray::array<unsigned int> indicebuffer;
using namespace grid;
const Vector3 vert[] = {
		 Vector3(0, 0, 0),//vertex 0
		 Vector3(1,0,0),//vertex 1
		 Vector3(1,1,0),//vertex 2
		  Vector3(0,1,0),//vertex 3
		  Vector3(0,0,1),//vertex 4
		  Vector3(1,0,1),//vertex 5
		  Vector3(1,1,1),//vertex 6
		  Vector3(0,1,1),//vertex 7

};
//uv's for cubemapping
const float cubeuv[] = {
   1, 0,
0, 0,
0, 1,
1, 1

};
bool chunkviewable(Chunk::chunk* chk) {

	v3::Vector3 center =chk->center()-camera::campos;


	for (size_t i = 0; i < 8; i++)
	{
		Vector3 norm = center + (vert[i]-unitv/2)* 16;
		if (dotproduct(norm,camera::direction())>0)
		{
			return true;
		}
	}
	return false;
}

v2::Vector2 facecoordsmcent(const face* fce, int ind) {
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

	v2::Vector2 uvCoord= v2::Vector2(cubeuv[2 * ind], cubeuv[2 * ind + 1]);
	v2::Vector2 ret = v2::unitv / 2 + offset * ((uvCoord - v2::unitv / 2) * -2);
	return ret;
}


//locatiion of unique indices in each set of vertices
const int uniqueindices[] = {
	5,1,2,6// east (+x)
	,0,4,7,3,// west (-x)
	2,3,7,6, // top (+y)
	0,1,5,4,// bottom (-y)
	4,5,6,7// south (+z)
   ,0,1,2,3// north (-z)
	
	
};
//0,1,2,3 are offsets from baselocation,this is a method to get the proper indices from offset
const int indiceoffsetfrombaselocation[]{
	0,1,2,0,2,3
};

void emitface(int face, block& torender,array<float>& datbuf,array<unsigned int>& indbuf) {

	if (!torender[face].covered)
	{
		int baselocation = datbuf.length / 7;
		for (int j = 0; j < 4; j++)
		{
			//index of uniqe vertice's in each face
			int uniqueind = uniqueindices[4 * face + j];
			//actual location
			//use *.9999 so clipping does not hapepen
			Vector3 offsetfromcenter = (vert[uniqueind] - unitv / 2) * Vector3((torender.mesh)->scale) * 2;
			Vector3 offset = torender.mesh->pos + offsetfromcenter;
			datbuf.append(offset.x);
			datbuf.append(offset.y);
			datbuf.append(offset.z);

			//2*j is x coord 2*j+1 is y coord
			v2::Vector2 coords = facecoordsmcent(&torender[face], j);
		
			
			datbuf.append(coords.x);
			float ytexpos = (cubeuv[2 * j + 1]);
			datbuf.append(coords.y);
			int texturenumb = (*(torender).mesh)[face].tex;
			datbuf.append(texturenumb);
			
				
				if (torender.transparent) {
					datbuf.append(torender.lightval);

				}
				else {

					datbuf.append((*torender.mesh)[face].light);
				}
			
		}

		for (int j = 0; j < 6; j++)
		{
			int indicelocation = baselocation + indiceoffsetfrombaselocation[j];
			indbuf.append(indicelocation);
		}
	}
}
void emitblock(block& torender, array<float>& datbuf, array<unsigned int>& indbuf) {
	if (torender.id != minecraftair)
	{


		for (int i = 0; i < 6; i++)
		{

			//set u to length ofver 6 because now 0-first new elem of bufffer

			//each vertice of the face
			

				emitface(i, torender, datbuf, indbuf);
			
		}
	}

}
void recreatechunkmesh(Chunk::chunk* aschunk) {
	aschunk->mesh->indbuf.destroy();
	aschunk->mesh->datbuf.destroy();
	aschunk->mesh->facebuf.destroy();
	aschunk->mesh->facebuf = dynamicarray::array<face>();
	aschunk->mesh->indbuf= dynamicarray::array<unsigned int>();
	aschunk->mesh->datbuf = dynamicarray::array<float>();
	

	for (int ind = 0;ind < chunksize;ind++) {

		block& blockatpos = (aschunk->blockstruct[ind]);
		if (blockatpos.transparent == false)
		{
			emitblock(blockatpos, aschunk->mesh->datbuf, aschunk->mesh->indbuf);
		}
		else
		{
			if (blockatpos.id != minecraftair)
			{


				for (int x = 0; x < 6; x++)
				{
					//each vertice of the face
					if (!(*blockatpos.mesh)[x].covered)
					{

						aschunk->mesh->facebuf.append((*blockatpos.mesh)[x]);
					}
			//implement sort elsewere
				}
			}
		}


	}

}

void renderchnk(chunkmesh& mesh,bool transparent)
{

	if (!transparent)
	{
		renderer::renderquadlist(mesh.Voa, mesh.ibo, mesh.VBO, mesh.datbuf, mesh.indbuf);
	}
	else
	{

		//enable 2d render
		array <float> datbuf = array<float>();
		array<unsigned int> indbuf = array<unsigned int>();
		
		for (int i = 0; i < mesh.facebuf.length; i++)
		{
			
			emitface(mesh.facebuf[i].facenum, *(mesh.facebuf[i].mesh->blk),datbuf,indbuf);
		}
		renderer::renderquadlist(mesh.Voa, mesh.ibo, mesh.VBO, datbuf, indbuf);
		datbuf.destroy();
		indbuf.destroy();
	}
}



void blockrender::initdatabuffer()
{
	for (int i = 0; i < totalgridsize; i++)
	{
		if (chunklist[i]->mesh->meshrecreateneeded) {
			recreatechunkmesh(chunklist[i]);
			chunklist[i]->mesh->meshrecreateneeded = false;
		
		}
		chunklist[i]->mesh->sortbuf();
	}
	array<Chunk::chunk> tosort = array < Chunk::chunk >();
	
	v3::Vector3 camerapos = camera::campos;
	
	for (int i = 0; i < totalgridsize; i++)
	{
		tosort.append(*(chunklist[i]));


	}
   oalgorithm::quicksort<Chunk::chunk>(tosort.getdata(), tosort.length);
   renderer::changerendertype(renderer::solid);
   renderer::setmat();

   for (int i = 0; i < totalgridsize; i++)
	{
	   if (chunkviewable(&tosort[i]))
	   {
		   renderchnk(*tosort[i].mesh, false);
	   }
	
	}
	
	renderer::changerendertype(renderer::transparent);
	for (int i = 0; i < totalgridsize; i++)
	{
		renderchnk(*tosort[i].mesh, true);
	}

	tosort.destroy();
	

}

