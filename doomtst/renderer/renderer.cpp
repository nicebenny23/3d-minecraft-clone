#include "renderer.h"
#include "../game/Core.h"
#include "../game/Settings.h"
#include "../game/GameContext.h"
namespace renderer {
   


    void Renderer::FillVertexBuffer(Mesh* mesh, stn::array<float>& pointlist)
    {
        if (pointlist.length % mesh->Voa.attributes.components() != 0)
        {
            throw std::logic_error("Vertex Data is corrupted");
        }
        if (!mesh->BuffersGenerated)
        {
            throw std::invalid_argument("Cannot Fill a mesh without Generating buffers first");
        }
        context.Bind(mesh->Vbo);
        mesh->Vbo.fillbuffer<float>(pointlist);
        context.Bind(mesh->Voa);
        mesh->Voa.SetAllAttributes();
        context.Bind(mesh->Ibo);
    }

    void Renderer::Fill(Mesh* mesh, stn::array<float>& pointlist, stn::array<unsigned int>& indicelist) {
      
        FillVertexBuffer(mesh, pointlist);
        context.Bind(mesh->Ibo);
        mesh->Ibo.fillbuffer<unsigned int>(indicelist);
        mesh->length = indicelist.length;

    }
    void Renderer::Fill(Mesh * mesh,stn::array<float>& pointlist) {
        array<unsigned int> indicelist = trivial_buffer(mesh->Voa.attributes, pointlist);
        Fill(mesh, pointlist, indicelist);
        indicelist.destroy();
    }
    void Renderer::Render(Mesh* mesh, stn::array<float>& pointlist)
    {
        Fill(mesh, pointlist);
        Render(mesh);
    }
    void Renderer::Render(Mesh* mesh, stn::array<float>& pointlist, stn::array<unsigned int>& indicelist)
    {
        Fill(mesh, pointlist, indicelist);
        Render(mesh);
    }


    void Renderer::Destroy(Mesh* mesh)
    {
        if (!mesh->BuffersGenerated)
        {
            throw std::invalid_argument("Cannot Delete a mesh without Generating buffers first");
        }
       
          
        Binders.Destroy(&mesh->Ibo);
        Binders.Destroy(&mesh->Voa);
        Binders.Destroy(&mesh->Vbo);
        mesh->BuffersGenerated = false;
        mesh->length = -1;
    }

    void Renderer::Render(Mesh* mesh) {

        if (!mesh->HasBeenFilled())
        {
            throw std::logic_error("Cannott render a UnbindedMesh Mesh");
        }
        context.Bind(mesh->Vbo);
      context.Bind(mesh->Voa);
        mesh->Voa.SetAllAttributes();
        context.Bind(mesh->Ibo);
        if (settings::Gamesettings.viewmode)
        {
            glDrawElements(GL_LINES, mesh->length, GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawElements(GL_TRIANGLES, mesh->length, GL_UNSIGNED_INT, 0);
        }
    }

    
   
 
   

    Renderer::Renderer()
    {
        Modes = MaterialManager(&Shaders,&uniform_man);
        fov = 90;
        view = glm::mat4(0);
        setprojmatrix(90, .21f, 100);
        Shaders.Compile("UiShader", "shaders\\uivertex.vs", "shaders\\uifragment.vs");
        Construct("Ui", "UiShader", RenderProperties(false, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
        uniforms::uparam("aspect_ratio","aspectratio")
        );
           
        Shaders.Compile("ModelShader","shaders\\modelvertex.vs", "shaders\\modelfragment.vs");
        Construct("Model", "ModelShader", RenderProperties(true, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
        uniforms::uparam("aspect_ratio","aspectratio"),
            uniforms::uparam("proj_matrix","projection"),
        uniforms::uparam("view_matrix","view_matrix")
        );

        Shaders.Compile("ParticleShader","shaders\\particlevertex.vs", "shaders\\particlefragment.vs");
        Construct("Particle", "ParticleShader", RenderProperties(true, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        
        Shaders.Compile("TextShader","shaders\\textvertex.vs", "shaders\\textfragment.vs");
        Construct("Text", "TextShader", RenderProperties(false, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
        uniforms::uparam("aspect_ratio","aspectratio")
        );

        set_uniform("aspect_ratio",CtxName::ctx.Window->AspectRatio());
        context = RenderContext::Context(&uniform_man);
       
              
   
    }

    void Renderer::setviewmatrix(glm::mat4 viewmat)
    {
            set_uniform("view_matrix",viewmat);
    }

    void Renderer::setprojmatrix(float newfov, float nearclipplane, float farclipplane)
    {

        set_uniform("proj_matrix",glm::perspective(glm::radians(newfov), float(4 / 3.f), nearclipplane, farclipplane));
        fov = newfov;
        
    }



    void Renderer::SetType(std::string Name)
    {

        context.bind(&Modes.get_material(Modes.get_id(Name)), this);
    }

  

    void Renderer::Clear()
    {

        set_uniform("aspect_ratio",CtxName::ctx.Window->AspectRatio());
        glClearColor(0, 0, 0, 0.0f);
        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthMask(GL_FALSE);
    }

    void Renderer::SetUniform(const std::string& name, float value)
    {
        CurrentShader()->setf(value, name.c_str());

    }
    void Renderer::SetUniform(const std::string& name,const glm::vec2 &vec)
    {
        CurrentShader()->SetVector2f(vec, name.c_str());
    }

    void Renderer::SetUniform(const std::string& name, const glm::vec3& vec)
    {
        CurrentShader()->SetVector3f(vec, name.c_str());

    }

    void Renderer::SetUniform(const std::string& name,const glm::vec4& vec)
    {

        CurrentShader()->SetVector4f(vec, name.c_str());
    }
    
    void Renderer::SetUniformMat4(const std::string& name, const glm::mat4& mat)
    {
        CurrentShader()->setMat4(mat, name.c_str());
    }

}
