#include "renderer.h"

namespace renderer {

    unsigned int VBO;
    unsigned int VAO;
    unsigned int ibo;//indicebufferobject
    dynamicarray::array<shader> shaderlist;
    glm::mat4 proj;
    glm::mat4 view;
    int currshader;
    texture blocktexture ;



    void renderquadlist(dynamicarray::array<float> &pointlist,dynamicarray::array<unsigned int> &indicelist)
    {
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*pointlist.length, pointlist.list, GL_STATIC_DRAW);

        glBindVertexArray(VAO);


        //uv
        glBindVertexArray(0);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indicelist.length, indicelist.list, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)0);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));

        glEnableVertexAttribArray(1);

        glBindVertexArray(ibo);

        glBindVertexArray(VAO);
        float g = pointlist[10];
        shaderlist[currshader].setvalmat(proj, "projection");
        shaderlist[currshader].setvalmat(view, "view");
        int vertexColorLocation = glGetUniformLocation(shaderlist[currshader].id, "Color");

     
        glDrawElements(GL_TRIANGLES, indicelist.length, GL_UNSIGNED_INT, 0);

      

    }

    void load()
    {
        view = glm::mat4(1);
        proj = glm::perspective(glm::radians(45.f), float(4 / 3), 0.1f, 100.0f);
        currshader = normal;
        shaderlist = dynamicarray::array<shader>(10);
        shaderlist[normal] = shader::shader("vert1.vs", "frag1.vs");
        shaderlist[normal].attach();
        glUseProgram(shaderlist[normal].id);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &ibo);
        blocktexture = texture::texture("coal.jpg", jpeg);
        glBindTexture(GL_TEXTURE_2D, blocktexture.id);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 10);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glCullFace(GL_BACK);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 1000);

       // GL_TEXTURE_MAX_LOD=1000;
    }

    void setviewmatrix(glm::mat4 viewmat)
    {
        view = viewmat;
    }

}
