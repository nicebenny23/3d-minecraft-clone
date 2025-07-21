#include "../util/SparseSet.h"
#include "Mesh.h"
#include "VertexObjectManager.h"
#pragma once

namespace Meshes {
	
	struct MeshRegistry {
		MeshRegistry(VObjMan::VObjManager* manager):man(manager) {

		}
		MeshRegistry() {

		}
	
		VObjMan::VObjManager* man;
		Sparse::KeySet<Mesh> meshes;
		stn::array<size_t> free;
		void remove(Ids::Id& handle) {
			auto& to_destroy = meshes[handle.id];
			man->Destroy(&to_destroy.Vbo);
			man->Destroy(&to_destroy.Ibo);
			man->Destroy(&to_destroy.Voa);
			meshes.erase_key(handle.id);
			free.push(handle.id);
			handle= Ids::None;
		}
		Ids::Id create() {
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
			return Ids::Id(id);
		}
		Mesh& operator[](Ids::Id key) {
			return meshes[key.id];
		}
	};
}