#include "chunkrender.h"
#include "../world/chunk.h"

void chunkmesh::genbufs()
{
	Voa.generate();
	VBO.generate(GL_ARRAY_BUFFER);
	ibo.generate(GL_ELEMENT_ARRAY_BUFFER);
}



int compare(const void* b, const void* a) {
	return sign((*(face*)a).cameradist - (*(face*)b).cameradist);
}
void chunkmesh::sortbuf()
{
	for (int i = 0; i < facebuf.length; i++)
	{
		facebuf[i].calccameradist();
	}
	std::qsort(facebuf.getdata() , facebuf.length, sizeof(face), compare);

}


void chunkmesh::destroy()
{
	Voa.destroy();
	ibo.destroy();
	VBO.destroy();
	datbuf.destroy();
	indbuf.destroy();
	facebuf.destroy();
}
