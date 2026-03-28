
#include "Mesh.h"
#include "RenderContext.h"
#include "../../util/Id.h"
#include "../renderer/vertex.h"
#pragma once

namespace renderer {
	using mesh_id = stn::rc<Mesh>;
	enum class indice_mode {
		generate_indices = 0,
		manual_generate = 1,
	};

	struct MeshData {

		size_t components() const {
			return layout.components();
		}
		size_t stride() const {
			return layout.stride();
		}
		size_t indices_count() const {
			return indices.length();
		}
		size_t vertex_count() const {
			return points.length()/layout.components();
		}
		MeshData(mesh_id msh, const vertice::vertex& vertex_layout, indice_mode indices) :mesh(msh), generate_trivial(indices), layout(vertex_layout), points() {
		}
		template<typename ...Args>
		inline void add_point(const Args& ...values) {
			size_t start_len = points.length();

			(push_single(values), ...);

			size_t end_len = points.length();
			size_t floats_pushed = end_len - start_len;

			if (components() != floats_pushed) {
				stn::throw_logic_error("mismatch: number of appended points {} does not match the stated number of components {}", floats_pushed, components());
			}

			if (generate_trivial == indice_mode::generate_indices) {
				indices.push(static_cast<size_t>(indices.length()));
			}

		}
		template<typename Range> requires convertible_range<Range, std::uint32_t>
		void add_indices(const Range& indice_list) {
			indices.append(indice_list);
		}
		void add_index(std::uint32_t index) {
			indices.push(index);
		}
		template<typename Range> requires convertible_range<Range, std::uint32_t>
		void add_indices_offset_from(const Range& indice_list, std::uint32_t offset) {
			for (auto i : indice_list) {
				indices.push(i + offset);
			}
		}
		mesh_id mesh;
		indice_mode generate_trivial;
		vertice::vertex layout;
		stn::array<float> points;
		stn::array<std::uint32_t> indices;
		
	private:
		inline void push_single(const v3::Point3& v) {
			points.push({ float(v.x),float(v.y),float(v.z) });
		}
		inline void push_single(const v2::Vec2& v) {
			points.push({ float(v.x),float(v.y) });
		}
		inline void push_single(float f) {
			points.push(f);
		}
	};
	struct MeshRegistry {

		MeshRegistry(renderer::Context* context) :ctx(context) {

		}
		MeshRegistry() :ctx(nullptr) {

		}

		renderer::Context* ctx;
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

		void load_in(MeshData& data) {
			Mesh& msh = *data.mesh;
			if (data.points.length() % data.layout.components() != 0) {
				throw std::logic_error("Vertex Data is corrupted");
			}
			if (msh.ebo.id) {
				ctx->bind(msh);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.length() * sizeof(GLuint), data.indices.data(), GL_STATIC_DRAW);
				glBufferData(GL_ARRAY_BUFFER, data.points.length() * sizeof(float), data.points.data(), GL_STATIC_DRAW);
				size_t stride = data.stride();
				size_t offset = 0;
				vertice::vertex& mesh_vertex = data.layout;
				for (int i = 0; i < mesh_vertex.length(); i++) {
					vertice::vertex_attribute& attribute = mesh_vertex[i];
					GlUtil::set_attr(i, attribute.components, attribute.type, GLsizei(stride), offset);
					glEnableVertexAttribArray(i);
					offset += attribute.size;
				}

				msh.length = data.indices_count();

			}
		}

	};
}