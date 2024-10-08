#include "renderer.h"
#include "../util/userinput.h"

namespace renderer {

    
    dynamicarray::array<shader> shaderlist;
    glm::mat4 proj;
    glm::mat4 view;
    int currshader;
    texturearray texarray;
    rendertype currendertype;
    float fov;
    void changerendertype(rendertype rentype) {

        if (rentype==currendertype)
        {
            return;
        }
        else
        {
            currendertype = rentype;
        }
        switch (rentype) {
        case rendersolid:
       
            break;
        case rendermodel:
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glDisable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_BLEND);
            currshader = modelshader;
            shaderlist[modelshader].attach();
            setrenderingmatrixes();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            
            break;
        case rendertransparent:
      break;
        case renderui:
           
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            currshader = uishader;
            shaderlist[uishader].attach();
            
            break;
        case renderparticle:

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            currshader = particleshader;
            shaderlist[particleshader].attach();
            setrenderingmatrixes();
            break;
        case rendertext:

            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            currshader = textshader;
            shaderlist[textshader].attach();
           
            glBindTexture(GL_TEXTURE_2D, 0);
            break;
        }
    }
    void renderquadlist(vao VAO,vbuf ibo,vbuf VBO,array<float> &pointlist,array<unsigned int> &indicelist)
    {
        
        VBO.bind();
       
        VBO.fillbuffer<float>(pointlist);
     


        //uv
        VAO.bind();
        VAO.set_attr(0, 3, GL_FLOAT, 7 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        //texture coords,inclusing the texture in the array 
        VAO.set_attr(1, 3, GL_FLOAT, 7 * sizeof(float), 3 * sizeof(float));
        glEnableVertexAttribArray(1);
        VAO.set_attr(2, 1, GL_FLOAT, 7 * sizeof(float), 6 * sizeof(float));
        glEnableVertexAttribArray(2);
        ibo.bind();
        ibo.fillbuffer<unsigned int>(indicelist);
        
        if (userinput::getinputkey('t').held)
        {
            glDrawElements(GL_LINES, indicelist.length, GL_UNSIGNED_INT, 0);
        }
       
        else
        {
            glDrawElements(GL_TRIANGLES, indicelist.length, GL_UNSIGNED_INT, 0);
        }
      


    }
    void renderquadlist(vao VAO, vbuf ibo, vbuf VBO, int size)
    {
        VBO.bind();



        //uv
        VAO.bind();
        VAO.set_attr(0, 3, GL_FLOAT, 7 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        //texture coords,inclusing the texture in the array 
        VAO.set_attr(1, 3, GL_FLOAT, 7 * sizeof(float), 3 * sizeof(float));
        glEnableVertexAttribArray(1);
        VAO.set_attr(2, 1, GL_FLOAT, 7 * sizeof(float), 6 * sizeof(float));
        glEnableVertexAttribArray(2);
        ibo.bind();
      
        if (userinput::getinputkey('t').held)
        {
            glDrawElements(GL_LINES, size, GL_UNSIGNED_INT, 0);
        }

        else
        {
            glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
        }


    }
    void prerenderquadlist(vao VAO, vbuf ibo, vbuf VBO, dynamicarray::array<float>& pointlist, dynamicarray::array<unsigned int>& indicelist)
    {

        VBO.bind();

        VBO.fillbuffer<float>(pointlist);



        //uv
        VAO.bind();
        VAO.set_attr(0, 3, GL_FLOAT, 7 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        //texture coords,inclusing the texture in the array 
        VAO.set_attr(1, 3, GL_FLOAT, 7 * sizeof(float), 3 * sizeof(float));
        glEnableVertexAttribArray(1);
        VAO.set_attr(2, 1, GL_FLOAT, 7 * sizeof(float), 6 * sizeof(float));
        glEnableVertexAttribArray(2);
        ibo.bind();
        ibo.fillbuffer<unsigned int>(indicelist);

    }
    void setrenderingmatrixes()
    {
        shaderlist[currshader].setmatval(proj, "projection");
        shaderlist[currshader].setmatval(view, "view");
    }
    void render2dquadlist(vao VAO, vbuf ibo, vbuf VBO, array<float>& pointlist, array<unsigned int>& indicelist)
    {
     
         VBO.bind();
       
        VBO.fillbuffer<float>(pointlist);
     


        //uv
        VAO.bind();
        VAO.set_attr(0, 2, GL_FLOAT, 4 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        //texture coords,inclusing the texture in the array 
        VAO.set_attr(1, 2, GL_FLOAT, 4 * sizeof(float), 2 * sizeof(float));
        glEnableVertexAttribArray(1);
        ibo.bind();
        ibo.fillbuffer<unsigned int>(indicelist);
         //enable position
        glDrawElements(GL_TRIANGLES, indicelist.length, GL_UNSIGNED_INT, 0);

      
    }
    void render2dtextarray(vao VAO, vbuf ibo, vbuf VBO, array<float>& pointlist, array<unsigned int>& indicelist)
    {
        VBO.bind();

        VBO.fillbuffer<float>(pointlist);



        //uv
        VAO.bind();
        VAO.set_attr(0, 2, GL_FLOAT, 5 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        //texture coords,inclusing the texture in the array 
        VAO.set_attr(1, 3, GL_FLOAT, 5* sizeof(float), 2 * sizeof(float));
        glEnableVertexAttribArray(1);
        ibo.bind();
        ibo.fillbuffer<unsigned int>(indicelist);
        //enable position
        glDrawElements(GL_TRIANGLES, indicelist.length, GL_UNSIGNED_INT, 0);

    }
    void setprojmatrix(float newfov,float nearclipplane, float farclipplane){
        proj = glm::perspective(glm::radians(newfov), float(4 / 3.f), nearclipplane, farclipplane);
        fov = newfov;
    }
    void generatetexarray() {


            
    }
   
    void load()
    {
        fov = 90;
        view = glm::mat4(0);
        setprojmatrix(90, .21f, 100);
        shaderlist = dynamicarray::array<shader>(1,false);
        shaderlist[uishader] = shader::shader("shaders\\uivertex.vs", "shaders\\uifragment.vs");
            shaderlist[uishader].attach();
        
       shaderlist[modelshader] = shader::shader("shaders\\modelvertex.vs", "shaders\\modelfragment.vs");
       shaderlist[modelshader].attach();
      
       shaderlist[particleshader] = shader::shader("shaders\\particlevertex.vs", "shaders\\particlefragment.vs");
       shaderlist[particleshader].attach();
 
       shaderlist[textshader] = shader::shader("shaders\\textvertex.vs", "shaders\\textfragment.vs");
       shaderlist[textshader].attach();
  
      
        
           
    
       
              generatetexarray();
            
   
    }

    void setviewmatrix(glm::mat4 viewmat)
    {
            view = viewmat;
    }

    void clear()
    {
          glClearColor(0, 0, 0, 0.0f);
            glDepthMask(GL_TRUE);
                   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         glDepthMask(GL_FALSE);

    }

}
