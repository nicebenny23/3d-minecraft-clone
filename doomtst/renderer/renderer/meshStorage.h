
#include "Mesh.h"
#include "RenderContext.h"
#include "../../util/Id.h"
#include "../renderer/vertex.h"
#include "../../util/reference.h"
#pragma once

namespace renderer {
	enum class indice_mode {
		generate_indices = 0,
		manual_generate = 1,
	};

	struct MeshBuilder {

		size_t components() const {
			return mesh.layout.components();
		}
		size_t stride() const {
			return mesh.layout.stride();
		}
		size_t indices_count() const {
			return mesh.indices.length();
		}
		size_t vertex_count() const {
			return mesh.points.length()/ mesh.layout.components();
		}
		MeshBuilder(mesh_id msh, const vertice::vertex& vertex_layout, indice_mode indices) :mesh{ .mesh=msh,.layout=vertex_layout }, generate_trivial(indices){
		}
		CpuMesh& built_mesh() {
			return mesh;
		}
		template<typename ...Args>
		inline void add_point(const Args& ...values) {
			size_t start_len = mesh.points.length();

			(push_single(values), ...);

			size_t end_len = mesh.points.length();
			size_t floats_pushed = end_len - start_len;

			if (components() != floats_pushed) {
				stn::throw_logic_error("mismatch: number of appended points {} does not match the stated number of components {}", floats_pushed, components());
			}

			if (generate_trivial == indice_mode::generate_indices) {
				mesh.indices.push(static_cast<size_t>(mesh.indices.length()));
			}

		}
		template<typename Range> requires convertible_range<Range, std::uint32_t>
		void add_indices(const Range& indice_list) {
			mesh.indices.append(indice_list);
		}
		void add_index(std::uint32_t index) {
			mesh.indices.push(index);
		}
		template<typename Range> requires convertible_range<Range, std::uint32_t>
		void add_indices_offset_from(const Range& indice_list, std::uint32_t offset) {
			for (auto i : indice_list) {
				mesh.indices.push(i + offset);
			}
		}
		indice_mode generate_trivial;

		CpuMesh mesh;
	private:
		inline void push_single(const v3::Point3& v) {
			mesh.points.push({ float(v.x),float(v.y),float(v.z) });
		}
		inline void push_single(const v2::Vec2& v) {
			mesh.points.push({ float(v.x),float(v.y) });
		}
		inline void push_single(float f) {
			mesh.points.push(f);
		}
	};
	struct MeshRegistry {

		MeshRegistry(renderer::Context& context) :ctx(context) {

		}
		stn::non_null<renderer::Context> ctx;
		stn::array<mesh_id> meshes;
		void remove_empty() {
			for (size_t i = 0; i < meshes.length();i++) {
				mesh_id& id = meshes[i];
				if (id.use_count()==1) {
					ctx->destroy(*id);
					meshes.swap_drop(i);
				}
			}
		}
		mesh_id create_mesh_data() {
			return meshes.emplace(ctx->create_mesh());
		}

		void load_in(MeshBuilder& data) {
			ctx->load(data.built_mesh());
		}

	};
}