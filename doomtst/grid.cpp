#include "grid.h"
#include <cmath>
using namespace v3;
namespace grid {

	float floorabs(float x) {

		if (x < 0)
		{
			return -1 * floor(-1 * x);
		}
		else
		{
			return floor(x);
		}
	}
	inline int modabs(int x, int m) {

		if (x < 0)
		{
			if (static_cast<double>(x) / m == int(x / m))
			{
				//so (-16,-16) is now a part of the correct chunk this has to bedon due to the inverse taken
				return 0;
			}
			else {
				return m - ((-x) % m);
			}
		}
		else
		{
			return x % m;
		}
	}
   Vector3 vert[] = {
		 Vector3(0, 0, 0),
		 Vector3(1,0,0),
		 Vector3(1,1,0),
		  Vector3(0,1,0),
		  Vector3(0,0,1),
		  Vector3(1,0,1),
		  Vector3(1,1,1),
		  Vector3(0,1,1),

	};
	//uv's for cubemapping
	const float cubeuv[] = {
	   1, 0,
	0, 0,
	0, 1,
	1, 1
	
	};
	//locatiion of unique indices in each set of vertices
	const float neededindice[] = { 
		0,1,2,5
		
	};
	//got this part on github
	int indices[] = { 1, 0, 3, 1, 3, 2, // north (-z)
		4, 5, 6, 4, 6, 7, // south (+z)
		5, 1, 2, 5, 2, 6, // east (+x)
		0, 4, 7, 0, 7, 3, // west (-x)
		2, 3, 7, 2, 7, 6, // top (+y)
		5, 4, 0, 5, 0, 1, }; // bottom (-y)
	
	

	Vector3 gridpos;
	Vector3 griddt;
	chunk::chunk** chunklist;
	dynamicarray::array<float> databuffer;
	dynamicarray::array<unsigned int> indicebuffer;
	
	void blockstruct(block& torender) {
		if (torender.id != minecraftair)
		{

			bool yf = (issolidatpos(torender.pos.x, torender.pos.y + 1, torender.pos.z));
			bool yb = (issolidatpos(torender.pos.x, torender.pos.y - 1, torender.pos.z));
			bool xf = (issolidatpos(torender.pos.x + 1, torender.pos.y, torender.pos.z));
			bool xb = (issolidatpos(torender.pos.x - 1, torender.pos.y , torender.pos.z));
			bool zf = (issolidatpos(torender.pos.x, torender.pos.y, torender.pos.z + 1));
			bool zb = (issolidatpos(torender.pos.x, torender.pos.y, torender.pos.z - 1));
		//if one of the sides around it is empty

		

				for (int i = 0; i < 6; i++)
				{
					bool willcontinue = true;
					switch (i)
					{
					case 0:
						willcontinue = zb;
						break;
					case 1:
						willcontinue = zf;
						break;
					case 2:
						willcontinue = xf;
						break;
					case 3:
						willcontinue = xb;
						break;
					case 4:
						willcontinue = yf;
						break;
					case 5:
						willcontinue = yb;
						break;
						
					}
					
					if (willcontinue)
					{
						//set u to length ofver 5 because now 0-first new elem of bufffer
						int baselocation = databuffer.length / 5;
						for (int j = 0; j < 4; j++)
						{
							//unique indices listed in order so you dont use all 6 only 4 indices
						//ind this side will be on
							
							int ind = 6 * i+neededindice[j];
							Vector3 offset = vert[indices[ind]] + torender.pos;
							databuffer.append(offset.x);
							databuffer.append(offset.y);
							databuffer.append(offset.z);

							//2*j is x coord 2*j+1 is y coord
							float xpos = cubeuv[2 * j] / texturesize + (6 * torender.texture + (i%texturesize)) / static_cast<float>(texturesize);
							databuffer.append(1-xpos);
							float ypos = (cubeuv[2 * j + 1] / texturesize) + floor((i + static_cast<float>(6) * torender.texture) / texturesize) / texturesize;
							databuffer.append(1-ypos );
						}
						for (int j = 0; j < 6; j++)
						{
						
							indicebuffer.append(baselocation + indices[j]);
						}
					}
					
				
			}
		}

	}

	void initdatabuffer() {
	
		for (int i = 0; i < (2*loadamt + 1) * (2*loadamt + 1); i++)
		{
			indicebuffer = dynamicarray::array<unsigned int>();
			databuffer = dynamicarray::array<float>();
			int ind = 0;
			for (int ind = 0;ind < 16*16*16;ind++) {
				
						block& block3 = ((*chunklist[i]).blockstruct[ind]);
						
						block3.pos;
							blockstruct(block3);

						
				
			}
			renderer::renderquadlist(databuffer, indicebuffer);
			indicebuffer.destroy();
			databuffer.destroy();
		}
	}
	

	block* getobjatgrid(int x, int y, int z)
	{

			int xchunk = x>>4;
			int zchunk = z >> 4;

		if (abs(xchunk-gridpos.x)<=loadamt&& abs(zchunk - gridpos.z) <= loadamt&&y<16&&0<=y)
		{
			//normolz=
			xchunk += loadamt- gridpos.x;
			zchunk += loadamt- gridpos.z;
			//get posisition in the chunk
			x = modabs(x,16);
			z = modabs(z,16);
		
			block* blockatpos = &(chunklist[xchunk + (2*loadamt+1)* zchunk])->blockstruct[256 * x + 16 * y + z];
			if (blockatpos->id !=minecraftair)
			{
				return blockatpos;
			}
			
		}
		return nullptr;
		
	}
	block* getobjatgrid2(int x, int y, int z)
	{

		int xchunk = x >> 4;
		int zchunk = z >> 4;

		if (abs(xchunk - gridpos.x) <= loadamt && abs(zchunk - gridpos.z) <= loadamt && y < 16 && 0 <= y)
		{
			//normolz=
			xchunk += loadamt - gridpos.x;
			zchunk += loadamt - gridpos.z;
			//get posisition in the chunk
			x = modabs(x, 16);
			z = modabs(z, 16);

			block* blockatpos = &(chunklist[xchunk + (2 * loadamt + 1) * zchunk])->blockstruct[256 * x + 16 * y + z];
			
				return blockatpos;
			

		}
		return nullptr;

	}
	bool issolidatpos(int x, int y, int z)
	{

		int xchunk = x >> 4;
		int zchunk = z >> 4;

		if (abs(xchunk - gridpos.x) <= loadamt && abs(zchunk - gridpos.z) <= loadamt && y < 16 && 0 <= y)
		{
			//normolz=
			xchunk += loadamt - gridpos.x;
			zchunk += loadamt - gridpos.z;
			//get posisition in the chunk
			x = modabs(x, 16);
			z = modabs(z, 16);

			if (chunklist[xchunk + (2 * loadamt + 1) * zchunk]->blockstruct[256 * x + 16 * y + z].id!=minecraftair)
			{
				return false;
			}

		}
		return true;

	}

	void initgrid()
	{
		 databuffer =dynamicarray::array<float>();
		gridpos = zerov;
		griddt = zerov;
		const int size = (2 * loadamt + 1);
		chunklist = new chunk::chunk*[size*size] ;
		for (int i = -loadamt; i <=loadamt; i++)
		{
			for (int j= - loadamt; j <= loadamt;j++)
			{
				chunklist[(i + loadamt)+(j + loadamt) * size] = chunk::load(i, j);

			}
		}
	}
	//order of storage for chunks & location-2loadamnt+1 is width and height
	//z
	//7,8,9
	//4,5,6
	//1,2,3 x
	void grid::load()
	{
		const int size = 2 * loadamt + 1;
		chunk::chunk** newchunklist =new chunk::chunk* [size*size];
		int indexdxchange = griddt.x + griddt.z * size;

		for (int i = 0; i < size; i++)
		{
			for (int j = 0;j < size;j++) {

				int ind = i + size * j;
				int nornmedx = chunklist[ind]->xchunk - gridpos.x;
				int nornmedz = chunklist[ind]->zchunk - gridpos.z;
				if (abs(nornmedx)<=loadamt&&abs(nornmedz)<=loadamt)
				{
					//moves to new posi
					newchunklist[ind - indexdxchange] = chunklist[ind];
				
				}
				else
				{
		//inv ind subtract one so 0 will go to (2*loadamt+1)^2-1
					int invind = (size*size)-(1 + ind);
					
				int x = (2 * loadamt  - i)+(gridpos.x-loadamt);
				int z = (2 * loadamt - j )+ (gridpos.z - loadamt);
				
				//gets 
				//	int z = floorabs((invind / static_cast<float>((2 * loadamt + 1)))) - loadamt + gridpos.z;
					newchunklist[invind] = chunk::load(x, z);

					chunklist[ind]->destroy();
					
				}
			
				
			}
		}
		delete[] chunklist;

		chunklist = newchunklist;

	}
	//get chunk space every frame
	//(0,0)->(0,0)
	//(16,0)->(1,0)
	void reupdatechunkborders()
	{
	
		Vector3 pos = Vector3(camera::campos);
		pos /= 16;
		pos.x =floor(pos.x);
		pos.z = floor(pos.z);
		griddt = pos-gridpos;
		gridpos = pos;
	}

}