
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



std::string  Chunk::getcorefilename(ChunkLocation pos) {

	std::string m = std::format("Chunk{}", pos.position);
	std::filesystem::path path = CtxName::ctx.wrld->get_path();
	path = path / "Chunks" / m;  // assign the combined path back
	return 	path.string();
}
//this whole system has to be completly redone
void appendspecialbytelist(array<unsigned short>& bytelist, int index, block* blk) {

}
void Chunk::chunk::write() {


	file_handle file = file_handle(getcorefilename(location.position), FileMode::ReadWriteBinary);
	array<unsigned short> bytelist = array<unsigned short >();
	for (int i = 0; i < chunksize; i++) {
		size_t v1 = static_cast<size_t>(block_list[i].get_component<block>().id);
		size_t dir = block_list[i].get_component<block>().mesh.direction.index();
		size_t attach = (block_list[i].get_component<block>().mesh.attached_direction.index());

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