#include "blockrender.h"
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
//locatiion of unique indices in each set of vertices
const float uniqueindices[] = {
	4,5,6,7// south (+z)
   ,0,1,2,3// north (-z)
	,1,2,6,5// east (+x)
	,0,4,7,3// west (-x)
	,2,3,7,6, // top (+y)
	0,1,5,4// bottom (-y)
};
//0,1,2,3 are offsets from baselocation,this is a method to get the proper indices from offset
const int indiceoffsetfrombaselocation[]{
	0,1,2,0,2,3
};

void emitface(int face, block& torender) {
	int baselocation = databuffer.length / 6;

	for (int j = 0; j < 4; j++)
	{
		//index of uniqe vertice's in each face
		int uniqueind = uniqueindices[4 * face + j];
		//actual location
		//use *.9999 so clipping does not hapepen
		Vector3 offset = vert[uniqueind]*.9999 + torender.pos;
		databuffer.append(offset.x);
		databuffer.append(offset.y);
		databuffer.append(offset.z);

		//2*j is x coord 2*j+1 is y coord

		float xtexpos = cubeuv[2 * j];
		databuffer.append(1 - xtexpos);
		float ytexpos = cubeuv[2 * j + 1];
		databuffer.append(1 - ytexpos);
		int texturenumb = torender[face].tex;
		databuffer.append(texturenumb);
	}

	for (int j = 0; j < 6; j++)
	{
		int indicelocation = baselocation + indiceoffsetfrombaselocation[j];
		indicebuffer.append(indicelocation);
	}

}

void emitblock(block& torender) {
	if (torender.id != minecraftair)
	{

		
		for (int i = 0; i < 6; i++)
		{
			bool willcontinue = !torender[i].covered;
			
	
			if (willcontinue)
			{
				//set u to length ofver 6 because now 0-first new elem of bufffer

				//each vertice of the face

				emitface(i, torender);
			}
		}
	}

}

array<face> transparentfaces;
void renderchunk(Chunk::chunk* rchunk) {
	indicebuffer = dynamicarray::array<unsigned int>();
	databuffer = dynamicarray::array<float>();
	int lenamount = 0;
	int startlen = transparentfaces.length;
	for (int ind = 0;ind < chunksize;ind++) {

		block& blockatpos = (rchunk->blockstruct[ind]);
		if (blockatpos.transparent==false)
		{
			emitblock(blockatpos);
		}
		else
		{
			if (blockatpos.id != minecraftair)
			{


				for (int x = 0; x < 6; x++)
				{
					lenamount++;
				
					transparentfaces.append(face(blockatpos[x]));
					transparentfaces.at(transparentfaces.length-1).calccameradist();
				}
			}
		}


	}
	renderer::renderquadlist(rchunk->mesh->Voa,rchunk->mesh->ibo,rchunk->mesh->VBO,databuffer, indicebuffer);
	
	oalgorithm::quicksort(transparentfaces.getdata() + startlen, lenamount);


	indicebuffer.destroy();
	databuffer.destroy();
}




void blockrender::initdatabuffer()
{
	array<Chunk::chunk> tosort = array < Chunk::chunk > ();
	transparentfaces = array<face>();
	v3::Vector3 camerapos = camera::campos;
	for (int i = 0; i < totalgridsize; i++)
	{
		tosort.append(*(chunklist[i]));


	}
	
	oalgorithm::quicksort(tosort.getdata(), tosort.length);
	for (int i = 0; i < totalgridsize; i++)
	{
		
		renderchunk(&tosort[i]);
	}
	glDisable(GL_CULL_FACE);
	indicebuffer = dynamicarray::array<unsigned int>();
	databuffer = dynamicarray::array<float>();
	for (int ind = 0;ind < transparentfaces.length;ind++) {
		emitface(transparentfaces[ind].facenum, *transparentfaces[ind].holder);
	}
	renderer::renderquadlist(chunklist[0]->mesh->Voa, chunklist[0]->mesh->ibo, chunklist[0]->mesh->VBO,databuffer,indicebuffer);
	indicebuffer.destroy();
	databuffer.destroy();
	transparentfaces.destroy();
	tosort.destroy();  

}
