
#include "shader.h"
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

shader::shader(const char* vertexpath, const char* fragpath)
{

	std::ifstream ffs(fragpath);//input file stream for fragment
	std::ifstream vfs(vertexpath);//file stram for vertex

	
	
    if (ffs.fail()||vfs.fail())
    {
               Assert("filestream error");
    }

    std::string fastr=std::string(std::istreambuf_iterator<char>(ffs), std::istreambuf_iterator<char>());
    std::string vastr = std::string(std::istreambuf_iterator<char>(vfs), std::istreambuf_iterator<char>());
    
    vfs.close();
    ffs.close();
    const char* vstr = vastr.c_str();
    const char* fstr = fastr.c_str();
    unsigned int vertexShader = glad_glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vstr, NULL);
    glCompileShader(vertexShader);
    
    GLint versuccess;
    char infolog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &versuccess);
    if (!versuccess)
    {

        glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
      
        std::cout << "vertex shader error ";
            Assert(infolog);
    }
GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fstr, NULL);
    glCompileShader(fragmentShader);
 
    GLint fragsuccess;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragsuccess);
    if (!fragsuccess)
    {
      
        glGetShaderInfoLog(fragmentShader, 512, NULL, infolog);
        std::cout << '/n' << "fragment shader error ";
        Assert(infolog);
    }
  
  
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glad_glLinkProgram(shaderProgram);
    GLint ssprs;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ssprs);
    if (ssprs !=GL_TRUE)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infolog);
        std::cout << '/n' << "fragment shader error ";
        Assert(infolog);
      
    }
    id = shaderProgram;
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
}


void shader::attach()
{
    glUseProgram(id);
}
