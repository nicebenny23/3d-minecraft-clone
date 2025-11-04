#include "GlUtil.h"
#pragma once
namespace vertice {
	struct vertex_attribute {
		size_t components; // Number of components (e.g., 3 for a vec3)
		GLenum type;     // The OpenGL type (e.g., GL_FLOAT, GL_INT)
		size_t size;

		vertex_attribute()
			: components(0), type(GL_NONE), size(0) {
		}

		vertex_attribute(GLenum Type, size_t Comps) {
			type = Type;
			size = Comps * GlUtil::Size(type);
			components = Comps;
			
		}
	};
	struct vertex {
		vertex() :total_size(), total_entries(), attributes() {

		}
		size_t length() const{
		return attributes.length();
		}
		size_t components() const {
			return total_entries;
		}
		size_t stride() const{
			return total_size;
		}
		vertex& push(GLenum Type, size_t components) {
			attributes.push(vertex_attribute(Type, components));
			total_entries += attributes.last().components;
			total_size += attributes.last().size;
			return *this;
		}
		template <typename T, size_t Comps>
		vertex& push() {
			return push(GlUtil::getGLType<T>(), Comps);
		}
		vertex_attribute& operator[](size_t index) {
			return attributes[index];

		}void Clear() {
			total_entries = 0;
			total_size = 0;
			attributes.clear();
		}
	private:
		size_t total_size;
		size_t total_entries;
		stn::array<vertex_attribute> attributes;
	};
}