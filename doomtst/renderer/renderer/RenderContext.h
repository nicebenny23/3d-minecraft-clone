#include "texture.h"
#include "shader.h"
#include "Uniform.h"
#include "ShaderManager.h"
#include "mesh.h"
#pragma once
namespace renderer {
	struct Renderer;
}
//Context For the Renderer which Controls Bindings And State
namespace renderer{

	//Context For the Renderer which Controls Bindings And State
	class RenderProperties {
	public:
		bool depthTestEnabled = true;     // Enable/Disable Depth Test
		bool depthWriteEnabled = true;    // Enable/Disable Depth Write
		bool blendingEnabled = false;     // Enable/Disable Blending
		bool cullFaceEnabled = true;      // Enable/Disable Face Culling
		GLenum blendFuncSrc = GL_SRC_ALPHA;   // Source blend function
		GLenum blendFuncDst = GL_ONE_MINUS_SRC_ALPHA; // Destination blend function

		// Default constructor
		RenderProperties() = default;

		// Constructor with parameters to set initial properties
		RenderProperties(bool depthTest, bool depthWrite, bool Cull, bool Blending, GLenum BlendFunctionSrc, GLenum BlendFunctionDst)
			: depthTestEnabled(depthTest),
			depthWriteEnabled(depthWrite),
			blendingEnabled(Blending),
			cullFaceEnabled(Cull),
			blendFuncSrc(BlendFunctionSrc),
			blendFuncDst(BlendFunctionDst)
		{
		}
		bool operator==(const RenderProperties& other) const = default;

	};
	struct Context
	{

		void bind(Mesh& mesh);
		void bind(Texture2D& Tex);

		void bind(TextureArray& Tex);
		void bind(shader& Shader);
		void unbind_mesh();
		void unbind_texture_2d();

		void unbind_texture_array();
		void unbind_shader();
		
		void set_uniform(uniforms::uniform value);
		

		
		Mesh create();
		void destroy(Mesh& msh);

		shader& bound_shader() {
			return BoundShader.unwrap();
		}
		
		void bind_properties(const RenderProperties& props);
		Context():BoundShader(), BoundTexture2d(),BoundTextureArray(), bound_mesh() { }
	
	private:
		RenderProperties properties;
		stn::Option<shader&> BoundShader;
		stn::Option <Texture2D> BoundTexture2d;
		stn::Option <TextureArray> BoundTextureArray;

		stn::Option <Mesh&> bound_mesh;


	};






}