#include "texture.h"
#include "shader.h"
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


	};
	struct Context
	{

		void bind(Mesh& mesh);
		void bind(ITexture& Tex);
		void bind(shader& Shader);
		void unbind_mesh();
		void unbind_texture();
		void unbind_shader();

		

		
		Mesh create();
		void destroy(Mesh& msh);

		shader& bound_shader() {
			return BoundShader.unwrap();
		}
		
		void bind_properties(const RenderProperties& props);
		Context():BoundShader(),BoundTexture(), bound_mesh(){}
	
	private:
		RenderProperties properties;
		stn::Option<shader&> BoundShader;
		stn::Option <ITexture&> BoundTexture;
		stn::Option <Mesh&> bound_mesh;


	};






}