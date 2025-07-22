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

    
    void Renderer::InitilizeBaseMaterials()
    {
        Shaders.Compile("UiShader", "shaders\\uivertex.vs", "shaders\\uifragment.vs");
        Construct("Ui", "UiShader", RenderProperties(false, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
            uniforms::uparam("aspect_ratio", "aspectratio")
        );

        Shaders.Compile("ModelShader", "shaders\\modelvertex.vs", "shaders\\modelfragment.vs");
        Construct("Model", "ModelShader", RenderProperties(true, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
            uniforms::uparam("aspect_ratio", "aspectratio"),
            uniforms::uparam("proj_matrix", "projection"),
            uniforms::uparam("view_matrix", "view_matrix")
        );

        Shaders.Compile("ParticleShader", "shaders\\particlevertex.vs", "shaders\\particlefragment.vs");
        Construct("Particle", "ParticleShader", RenderProperties(true, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        
        meshes = Meshes::MeshRegistry(&Binders);
        set_uniform("aspect_ratio", CtxName::ctx.Window->AspectRatio());
    }
 
   

    Renderer::Renderer()
    {
        Modes = MaterialManager(&Shaders,&uniform_manager);
        fov = 90;
        setprojmatrix(90, .21f, 100);
        meshes = Meshes::MeshRegistry();
    }

    void Renderer::setprojmatrix(float newfov, float nearclipplane, float farclipplane)
    {

        set_uniform("proj_matrix",glm::perspective(glm::radians(newfov), float(4 / 3.f), nearclipplane, farclipplane));
       
        
    }



    void Renderer::SetType(std::string Name)
    {

        bind_material(Modes.get_id(Name));

    }
    void Renderer::bind_material(Ids::Id material)
    {
        const Material& mat = Modes.get_material(material);
        context.Bind(Shaders.get_shader(mat.shader));
        for (const auto& elem : mat.handles)
        {
            apply_uniform(uniform_manager.get( elem), elem.name);
        }
        context.bind_properties(mat.prop);
    }
  
    void Renderer::apply_uniform(uniforms::uniform_val val, const char* location_in_shader)
    {
        
        switch (val.index()) {
        case uniforms::uform_int:
            context.bound_shader().setint(std::get<int>(val), location_in_shader);
            break;
        case uniforms::uform_float:
            context.bound_shader().setf(std::get<float>(val), location_in_shader);
            break;
        case uniforms::uform_gluint:
            context.bound_shader().setuint(std::get<unsigned int>(val), location_in_shader);
            break;
        case uniforms::uform_vec2:
            context.bound_shader().SetVector2f(std::get<glm::vec2>(val), location_in_shader);
            break;
        case uniforms::uform_vec3:
            context.bound_shader().SetVector3f(std::get<glm::vec3>(val), location_in_shader);
            break;
        case uniforms::uform_vec4:
            context.bound_shader().SetVector4f(std::get<glm::vec4>(val), location_in_shader);
            break;
        case uniforms::uform_mat3:
            context.bound_shader().setMat3(std::get<glm::mat3>(val), location_in_shader);
            break;
        case uniforms::uform_mat4:
            context.bound_shader().setMat4(std::get<glm::mat4>(val), location_in_shader);
            break;
        case uniforms::uform_tex:
            Textures.get_texture(std::get<Ids::Id>(val)).apply();
            break;
        default:
            throw std::logic_error("Invalid uniform paramater");
            break;
        }
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

    void RenderableHandle::set_material(const std::string& name)
    {
        renderer->set_material(id, name);
    }

    void RenderableHandle::set_layout(vertice::vertex layout)
    {
        renderer->set_layout(id, layout);
    }

    void RenderableHandle::fill(MeshData&& new_mesh)
    {
        renderer->fill_cmd(id,new_mesh);
    }

    void RenderableHandle::set_uniform(const uniforms::uniform& u)
    {
        renderer->set_uniform(id, u);
    }

    void RenderableHandle::render()
    {
        renderer->render(id);
    }

    void RenderableHandle::destroy()
    {
        renderer->remove(id);
    }

}
