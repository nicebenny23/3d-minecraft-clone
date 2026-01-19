#include "../../util/SparseSet.h"
#include "Mesh.h"
#include "RenderContext.h"
#include "../../util/Id.h"
#pragma once

namespace renderer {
	struct mesh_id_tag;
	using mesh_id = stn::typed_id<mesh_id_tag>;
	struct MeshRegistry {
	
		MeshRegistry(renderer::Context* context):ctx(context) {

		}
		MeshRegistry():ctx(nullptr){

		}
	
		renderer::Context* ctx;
		Sparse::KeySet<Mesh> meshes;
		stn::array<std::uint32_t> free;
		void remove(mesh_id& handle) {
			uint32_t id = handle.id;
			ctx->destroy(meshes[id]);
			meshes.erase_key(handle.id);
			free.push(id);
		}
		mesh_id create() {
			if (free.length()==0)
			{
				free.push(static_cast<std::uint32_t>(meshes.length()));
			}
			std::uint32_t id = free.pop();
			Mesh new_mesh = ctx->create_mesh();
			meshes.push(id, new_mesh);
			return mesh_id(id);
		}
		Mesh& operator[](mesh_id key) {
			return meshes[key.id];
		}
		Option<Mesh&> get(mesh_id key) {
			return meshes.get(key.id);
		}

	};
}