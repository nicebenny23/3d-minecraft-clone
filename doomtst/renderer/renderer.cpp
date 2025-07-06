#include "renderer.h"
#include "../game/Core.h"
#include "../game/Settings.h"
#include "../game/GameContext.h"
namespace renderer {
    void setAspectRatio(renderer::Renderer* renderer) {
        renderer->CurrentShader()->setf(CtxName::ctx.Window->AspectRatio(), "aspectratio");
    }
    void setrenderingmatrixes(renderer::Renderer* renderer)
    {

        renderer->CurrentShader()->setMat4(renderer->proj, "projection");
        renderer->CurrentShader()->setMat4(renderer->view, "view");
    }


    void Renderer::FillVertexBuffer(Mesh* mesh, Cont::array<float>& pointlist)
    {
        if (pointlist.length % mesh->Voa.ComponentsPerVertex() != 0)
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

    void Renderer::Fill(Mesh* mesh, Cont::array<float>& pointlist, Cont::array<unsigned int>& indicelist) {
        if (!mesh->IsEboMesh)
        {
            throw std::invalid_argument("attempted to intiate a non Ebo mesh with an indice list");
        }
        FillVertexBuffer(mesh, pointlist);
        context.Bind(mesh->Ibo);
        mesh->Ibo.fillbuffer<unsigned int>(indicelist);
        mesh->length = indicelist.length;

    }
    void Renderer::Fill(Mesh * mesh,Cont::array<float>& pointlist) {
        if (mesh->IsEboMesh)
        {
            throw std::invalid_argument("attempted to intiate an Ebo Mesh without an indice list");
        }
        FillVertexBuffer(mesh, pointlist);
        mesh->length = pointlist.length/mesh->Voa.ComponentsPerVertex();
    }

    void Renderer::Destroy(Mesh* mesh)
    {
        if (!mesh->BuffersGenerated)
        {
            throw std::invalid_argument("Cannot Delete a mesh without Generating buffers first");
        }
        if (mesh->IsEboMesh)
        {
            Binders.Destroy(&mesh->Ibo);
        }
        Binders.Destroy(&mesh->Voa);
        Binders.Destroy(&mesh->Vbo);
        mesh->BuffersGenerated = false;
        mesh->length = -1;
        mesh->IsEboMesh = true;
    }

    void Renderer::Render(Mesh* mesh) {

        if (!mesh->HasBeenFilled())
        {
            throw std::logic_error("Cannott render a UnbindedMesh Mesh");
        }
        context.Bind(mesh->Vbo);
      context.Bind(mesh->Voa);
        mesh->Voa.SetAllAttributes();
        if (mesh->IsEboMesh)
        {
            context.Bind(mesh->Ibo);
        }
        if (settings::Gamesettings.viewmode)
        {
            glDrawElements(GL_LINES, mesh->length, GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawElements(GL_TRIANGLES, mesh->length, GL_UNSIGNED_INT, 0);
        }
    }

    void Renderer::Render(Mesh* mesh, Cont::array<float>& pointlist)
    {
        Fill(mesh, pointlist);
        Render(mesh);
    }
    void Renderer::Render(Mesh* mesh, Cont::array<float>& pointlist, Cont::array<unsigned int>& indicelist)
    {
        Fill(mesh, pointlist, indicelist);
        Render(mesh);
    }
   
 
   

    Renderer::Renderer(size_t tst)
    {
        fov = 90;
        view = glm::mat4(0);
        setprojmatrix(90, .21f, 100);
        Shaders.Compile("UiShader", "shaders\\uivertex.vs", "shaders\\uifragment.vs");
       Base_Material Ui= Base_Material("Ui", "UiShader", RenderProperties(false, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)).AddUniform(setAspectRatio);
       AddType(Ui);
           
        Shaders.Compile("ModelShader","shaders\\modelvertex.vs", "shaders\\modelfragment.vs");

        AddType(Base_Material("Model", "ModelShader", RenderProperties(true, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)).AddUniform(setAspectRatio).AddUniform(setrenderingmatrixes));
        Shaders.Compile("ParticleShader","shaders\\particlevertex.vs", "shaders\\particlefragment.vs");
        AddType(Base_Material("Particle", "ParticleShader", RenderProperties(true, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)));
        Shaders.Compile("TextShader","shaders\\textvertex.vs", "shaders\\textfragment.vs");
        AddType(Base_Material("Text", "TextShader", RenderProperties(false, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)).AddUniform(setAspectRatio));
           
    
       
              
   
    }

    void Renderer::setviewmatrix(glm::mat4 viewmat)
    {
            view = viewmat;
    }

    void Renderer::setprojmatrix(float newfov, float nearclipplane, float farclipplane)
    {
        proj = glm::perspective(glm::radians(newfov), float(4 / 3.f), nearclipplane, farclipplane);
        fov = newfov;

    }



    void Renderer::SetType(std::string Name)
    {

        properties = Modes[Name];
        context.Bind(Shaders[properties.Shader]);
        for (int i = 0; i < properties.UniformsCalls.length; i++)
        {
            properties.UniformsCalls[i](this);
        }
            
        applyProperties();
    }

    void Renderer::applyProperties()
    {
       
        glDepthMask(properties.prop.depthWriteEnabled );
        GlUtil::SetProperty(GL_CULL_FACE,properties.prop.cullFaceEnabled);
        GlUtil::SetProperty(GL_DEPTH_TEST,properties.prop.depthTestEnabled);
        glBlendFunc(properties.prop.blendFuncSrc, properties.prop.blendFuncDst);
        GlUtil::SetProperty(GL_BLEND, properties.prop.blendingEnabled);
    }

    void Renderer::Clear()
    {
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
