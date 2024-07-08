#include "block.h"
#include "algorthm.h"
#include "renderer.h"
using namespace blockname;
#ifndef chunkrender_H
#define chunkrender_H


struct chunk;
struct chunkmesh
{
	chunkmesh() = default;
	vbuf VBO;
	vbuf ibo;
	vao Voa;
	//voa vertexspec;
	void genbufs();
	
	bool meshrecreateneeded;
	chunk* aschunk;
	array<float> datbuf;
	array<unsigned int> indbuf;
	array<face> facebuf;
	void sortbuf();

	//todo  get basic mesh and        reupdsted working


	void destroy();
};
void createchunkmesh(chunk* aschunk);
#endif // !chunkrender_H

#pragma once
