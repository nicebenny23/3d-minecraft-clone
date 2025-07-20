#include "../util/SparseSet.h"
#include "Mesh.h"
#include "VertexObjectManager.h"
#pragma once
namespace Meshes {
	extern struct Renderer;
	struct meshId {
		size_t id;
		bool operator ==(const meshId oth) const{
			return id == oth.id;
		}
		bool operator !=(const meshId oth) const {
			return id != oth.id;
		}
	};
	struct MeshRegistry {
		Renderer* ren;
		VObjMan::VObjManager* man;
		Sparse::KeySet<Mesh> meshes;
		stn::array<size_t> free;
		void remove(meshId& handle) {
			auto& to_destroy = meshes[handle.id];
			man->Destroy(&to_destroy.Vbo);
			man->Destroy(&to_destroy.Ibo);
			man->Destroy(&to_destroy.Voa);
			meshes.erase_key(handle.id);
			free.push(handle.id);
			handle.id = -1;
		}
		meshId create() {
			if (free.length==0)
			{
				free.push(meshes.length());
			}
			size_t id = free.pop();
			Mesh new_mesh;
			man->Create(&new_mesh.Vbo);
			man->Create(&new_mesh.Ibo);
			man->Create(&new_mesh.Voa);
			new_mesh.BuffersGenerated = true;
			meshes.push(id, new_mesh);
			return meshId{ id };
		}
		Mesh& operator[] (meshId key) {
			return meshes[key.id];
		}
	};
}