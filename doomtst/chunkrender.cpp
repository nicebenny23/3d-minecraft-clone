#include "chunkrender.h"
#include "chunk.h"
void chunkmesh::genbufs()
{
	Voa.generate();
	VBO.generate(GL_ARRAY_BUFFER);
	ibo.generate(GL_ELEMENT_ARRAY_BUFFER);
}
void chunkmesh::sort()
{
	
}
void chunkmesh::createarrays()
{
}

void chunkmesh::destroy()
{
	Voa.destroy();
	ibo.destroy();
	VBO.destroy();
}
