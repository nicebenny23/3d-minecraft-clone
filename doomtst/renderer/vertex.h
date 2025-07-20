
#pragma once
namespace vertice {
	struct VertexAttribute {

		GLint Components; // Number of components (e.g., 3 for a vec3)
		GLenum Type;     // The OpenGL type (e.g., GL_FLOAT, GL_INT)
		VertexAttribute()
			: Components(0), Type(GL_NONE), size(0) {
		}

		VertexAttribute(GLenum type, GLint Comps) {
			size = Comps * GlUtil::Size(type);
			Components = Comps;
			Type = type;
		}

		// Calculate the size of the attribute in bytes
		size_t Size() const {
			return size;
		}
	private:
		size_t size;
	};
	struct vertex {
			size_t length() {
			return attributes.length;
			}
		size_t components() const {
			size_t sum=0;
			for (auto& atrib :attributes )
			{
				sum += atrib.Components;
			}
			return sum;
		}
		size_t stride() const{
			size_t sum = 0;
			for (auto& atrib : attributes)
			{
				sum += atrib.Size();
			}
			return sum;
		}
		template <typename T, GLint Components>
		vertex& push() {
			attributes.push(vertice::VertexAttribute(GlUtil::getGLType<T>(), Components));
			return *this;
		}
		VertexAttribute& operator[](size_t index) {
			return attributes[index];

		}
		vertex& push(GLenum Type, GLint Components) {
			attributes.push(VertexAttribute(Type, Components));
			return *this;
		}

		void Clear() {
			attributes.destroy();
		}
	private:
		stn::array<VertexAttribute> attributes;
	};
}