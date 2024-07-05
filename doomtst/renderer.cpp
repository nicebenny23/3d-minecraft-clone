#include "renderer.h"

namespace renderer {

    vbuf VBO;
    vao VAO;
    
    vbuf ibo;//indicebufferobject
    dynamicarray::array<shader> shaderlist;
    glm::mat4 proj;
    glm::mat4 view;
    int currshader;
    texture blocktexture ;



    void renderquadlist(dynamicarray::array<float> &pointlist,dynamicarray::array<unsigned int> &indicelist)
    {
        
        VBO.bind();
       
        VBO.fillbuffer<float>(pointlist);
     


        //uv

        ibo.bind();
        ibo.fillbuffer<unsigned int>(indicelist);
         //enable position
        VAO.bind();
           VAO.set_attr(0,3,GL_FLOAT,6*sizeof(float),0);
           glEnableVertexAttribArray(0);
        //texture coords,inclusing the texture in the array 
           VAO.set_attr(1, 3, GL_FLOAT, 6 * sizeof(float), 3*sizeof(float));
        glEnableVertexAttribArray(1);

      
 
        shaderlist[currshader].setmatval(proj, "projection");
        shaderlist[currshader].setmatval(view, "view");

        int vertexColorLocation = glGetUniformLocation(shaderlist[currshader].id, "Color");

     
        glDrawElements(GL_TRIANGLES, indicelist.length, GL_UNSIGNED_INT, 0);

      

    }
    void createtexturearray() {


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
        texlist[5] = "water.png";
        texlist[4] = "glass.png";
        texturearray texarr = texturearray(16, 16, texlist, png);
        texarr.apply();
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    void settextureparams() {
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, 10);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glCullFace(GL_BACK);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LOD, 1000);
    }
    void load()
    {
        view = glm::mat4(0);
        setprojmatrix(70, .1, 100);
        currshader = normal;
        shaderlist = dynamicarray::array<shader>(10);
        shaderlist[normal] = shader::shader("vert1.vs", "frag1.vs");
        shaderlist[normal].attach();
        glUseProgram(shaderlist[normal].id);

        VAO.generate();
        VBO.generate(GL_ARRAY_BUFFER);
        ibo.generate(GL_ELEMENT_ARRAY_BUFFER);
    
     
        //blocktexture = texture::texture("coal.jpg", jpeg);
        //blocktexture.apply();
        generatetexarray();
        settextureparams();

       // GL_TEXTURE_MAX_LOD=1000;
    }

    void setviewmatrix(glm::mat4 viewmat)
    {
        view = viewmat;
    }

}
