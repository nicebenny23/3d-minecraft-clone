
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
#include "../game/GameContext.h"


//must be a valid index
size_t Chunk::indexfrompos(Coord pos) {

	int x = symmetric_mod(pos.x, chunkaxis);
	int y = symmetric_mod(pos.y, chunkaxis);
	int z = symmetric_mod(pos.z, chunkaxis);
	return	chunkaxis * chunkaxis * x + chunkaxis * y + z;
}








std::string  Chunk::getcorefilename(Coord pos) {

	std::string m = std::format("Chunk{}", pos);
	std::filesystem::path path = CtxName::ctx.wrld->get_path();
	path = path / "Chunks" / m;  // assign the combined path back
	return 	path.string();
}
//this whole system has to be completly redone
void appendspecialbytelist(array<unsigned short>& bytelist, int index, block* blk) {

}
void Chunk::chunk::write() {


	file_handle file = file_handle(getcorefilename(loc), FileMode::ReadWriteBinary);
	array<unsigned short> bytelist = array<unsigned short >();
	for (int i = 0; i < chunksize; i++) {
		size_t v1 = block_list[i].get_component<block>().block_id;
		size_t dir = block_list[i].get_component<block>().mesh.direction.ind();
		size_t attach = (block_list[i].get_component<block>().mesh.attachdir.ind());

		size_t v2 = dir | attach << 3;
		size_t fin = v1 | (v2 << 8);
		if (dir > 5 || attach > 5) {
			throw std::logic_error("Directional corruption error");
		}
		bytelist.push(fin);
	}
	for (int i = 0; i < chunksize; i++) {
		bytelist.reach(chunksize + i) = 0;
		appendspecialbytelist(bytelist, i, block_list[i].get_component_ptr<block>());
	}
	file.write<unsigned short>(bytelist.span());

	file.close();
}



void Chunk::chunk::destroy() {
	owner().destroy();
	if (modified) {
		write();
	}
	for (int i = 0; i < chunksize; i++) {
		std::move(block_list[i]).destroy();
	}
	
	block_list.clear();
}