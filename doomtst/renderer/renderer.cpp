#include "renderer.h"

namespace renderer {

    vbuf VBO;
    vao VAO;
    
    vbuf ibo;//indicebufferobject
    dynamicarray::array<shader> shaderlist;
    glm::mat4 proj;
    glm::mat4 view;
    int currshader;
    texturearray texarray;
   
    void changerendertype(rendertype rentype) {

        switch (rentype) {
        case solid:
           glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
         glCullFace(GL_BACK);
              glDisable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_BLEND);
           currshader = normal;
            shaderlist[normal].attach();
           glUseProgram(shaderlist[normal].id);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
          texarray.apply();
            break;
        case transparent:
           glDepthFunc(GL_LESS);
            glDepthMask(GL_FALSE);
            glDisable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
           currshader = normal;
          shaderlist[normal].attach();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glUseProgram(shaderlist[normal].id);
           texarray.apply();
            
            break;
        case ui:
           
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            currshader = uishader;
            shaderlist[uishader].attach();
            glUseProgram(shaderlist[currshader].id);
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
         //enable position
       
 
        shaderlist[currshader].setmatval(proj, "projection");
        shaderlist[currshader].setmatval(view, "view");

        int vertexColorLocation = glGetUniformLocation(shaderlist[currshader].id, "Color");

     
        glDrawElements(GL_TRIANGLES, indicelist.length, GL_UNSIGNED_INT, 0);

      

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
    void setprojmatrix(float fov,float nearclipplane, float farclipplane){
        proj = glm::perspective(glm::radians(fov), float(4 / 3), nearclipplane, farclipplane);
    }
    void generatetexarray() {

        array<const char*> texlist = array<const char*>(0);
        texlist[0] = "dirt.png";
        texlist[1] = "grass.png";
        texlist[2] = "wood.png";
        texlist[3] = "woodint.png";
       
        texlist[4] = "glass.png";
        texlist[5] = "water.png";
        texlist[6] = "torch.png";
        texlist[7] = "torchtop.png";
       texarray = texturearray(16, 16, texlist);
        texarray.apply();
        
    }
    void settextureparams() {
      //  glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, 10);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
     

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LOD, 1000);
    }
    void load()
    {
        view = glm::mat4(0);
        setprojmatrix(90, .3, 100);
        shaderlist = dynamicarray::array<shader>(10);
        shaderlist[uishader] = shader::shader("uivertex.vs", "uifragment.vs");
       shaderlist[uishader].attach();

        shaderlist[normal] = shader::shader("vert1.vs", "frag1.vs");
        shaderlist[normal].attach();
              currshader = normal;
        glUseProgram(shaderlist[normal].id);
        
        
    
       
        generatetexarray();
        settextureparams();

   
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
