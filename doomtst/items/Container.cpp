#include "Container.h"
#include "../util/fileloader.h"
int currentcontid;
void deleteallcontainerfiles()
{
	currentcontid = 0;
}

const char* writefilename(int id)
{

	std::string* strng = (new std::string());
	strng->append("worldstorage\\");
	strng->append("Container");
	strng->append(std::to_string(id));
	return 	strng->data();
}

Container::Container(int newid)
{
	
	containerid = newid;
	const char* filename = writefilename(newid);
	safefile contfile = safefile(filename, fileread);
	float* sizeamts=  contfile.read<float>(4);
	sizex = sizeamts[0];
	sizey = sizeamts[1];
	offset.x= sizeamts[2];
	offset.y= sizeamts[3];

	delete[] sizeamts;
	contfile.go(sizeof(float) * 4);
	short* objdata = contfile.read<short>(size_t(sizex) * size_t(sizey));
int ind = 0;
	for (int i = 0; i < sizex; i++)
	{
		for (int j = 0; j < sizey; j++)
		{

			databuf[ind] = itemslot(i + offset.x- float(sizex) / 2.0f, j + offset.y- sizey/ 2.0f);
			ind++;
		}
	}
	disable();
	for (int i = 0; i < sizex*sizey; i++)
	{
		databuf[i].giveitem(objdata[i] & 255);
		databuf[i].helditem->amt= objdata[i] >>8;
	}
	delete[] objdata;
	contfile.close();
}

void Container::writetofile()
{

	const char* filename = writefilename(containerid);
	safefile contfile = safefile(filename, filewrite);
	float* sizeamts = new float [4];
	 sizeamts[0] = sizex;

	sizeamts[1]=sizey;
	sizeamts[2]=offset.x;
	 sizeamts[3]= offset.y;
	 contfile.write<float>(sizeamts, 4);
	 contfile.go(sizeof(float) * 4);
	 delete[] sizeamts;
	short* objdata = new short[size_t(sizex) * size_t(sizey)];
	
	for (int ind = 0; ind< sizex*sizey; ind++)
	{
			if (databuf[ind].helditem == nullptr)
			{
				objdata[ind] = 0;
				continue;
			}
			int idval = databuf[ind].helditem->id;
			objdata[ind] = (databuf[ind].helditem->amt * 256) | idval;
		
	}
	contfile.write<short>(objdata, size_t( sizex )* size_t( sizey));
	contfile.close();
	delete[] objdata;
}
