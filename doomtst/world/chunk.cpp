
#include "chunk.h"
#include "noise.h"
#include <string>
#include "../util/fileloader.h"
#include "../block/blockinit.h"
#include "../block/block.h"
#include <format>
#include "../util/algorthm.h"#include "../util/dynamicarray.h"
#include "../renderer/renderer.h"
#include "../math/vector3.h"
ecs::obj& Chunk::chunk::operator[](size_t index) {
	return blockbuf[index];
}


//must be a valid index
size_t Chunk::indexfrompos(Coord pos) {

	int x = symmetric_mod(pos.x, chunkaxis);
	int y = symmetric_mod(pos.y, chunkaxis);
	int z = symmetric_mod(pos.z, chunkaxis);
	return	chunkaxis * chunkaxis * x + chunkaxis * y + z;
}


void Chunk::chunkmesh::genbufs() {
	SolidGeo = CtxName::ctx.Ren->gen_renderable();
	SolidGeo.set_material("SolidBlock");
	SolidGeo.set_layout(vertice::vertex().push<float, 3>().push<float, 3>().push<float, 1>());
	TransparentGeo = CtxName::ctx.Ren->gen_renderable();
	TransparentGeo.set_material("TransparentBlock");
	
	TransparentGeo.set_layout(vertice::vertex().push<float, 3>().push<float, 3>().push<float, 1>());

}



void Chunk::chunkmesh::sort_faces() {
	std::sort(faces.begin(), faces.end(), [](face& a, face& b) {	
		return  v3::dist2(b.center(), camera::campos())< v3::dist2(a.center(), camera::campos());
	});
}


void Chunk::chunkmesh::destroy() {
	SolidGeo.destroy();
	TransparentGeo.destroy();
	faces.clear();
}



std::string  Chunk::getcorefilename(Coord pos) {

	std::string m = std::format("Chunk{}", pos);
	std::filesystem::path path = CtxName::ctx.wrld->get_path();
	path = path / "Chunks" / m;  // assign the combined path back
	return 	path.string();
}
//this whole system has to be completly redone
void appendspecialbytelist(array<unsigned short>& bytelist, int index, block* blk) {

	liquidprop* getliq = blk->owner().get_component_ptr<liquidprop>();
	if (getliq != nullptr) {
		bytelist[chunksize + index] = getliq->liqval;

	}
}
void Chunk::chunk::write() {


	file_handle file = file_handle(getcorefilename(loc), FileMode(true,true));
	array<unsigned short> bytelist = array<unsigned short >();
	for (int i = 0; i < chunksize; i++) {
		size_t v1 = blockbuf[i].get_component<block>().id;
		size_t dir = blockbuf[i].get_component<block>().mesh.direction.ind();
		size_t attach = (blockbuf[i].get_component<block>().mesh.attachdir.ind());

		size_t v2 = dir | attach << 3;
		size_t fin = v1 | (v2 << 8);
		if (dir > 5 || attach > 5) {
			throw std::logic_error("Directional corruption error");
		}
		bytelist.push(fin);
	}
	for (int i = 0; i < chunksize; i++) {
		bytelist.reach(chunksize + i) = 0;
		appendspecialbytelist(bytelist, i, blockbuf[i].get_component_ptr<block>());
	}
	file.write<unsigned short>(bytelist.span());

	file.close();
}

Chunk::chunk::chunk() :mesh(*this) {
	mesh->genbufs();
	loc = ZeroCoord;
	modified = false;
}

void Chunk::chunk::destroy() {
	if (modified) {
		write();
	}
	for (int i = 0; i < chunksize; i++) {
		std::move(blockbuf[i]).destroy();
	}
	mesh->destroy();
	blockbuf.clear();
}