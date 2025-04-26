
#include "shader.h"
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
void shaderstatuscheck(int id, GLint statustype,const char* name) {
    GLint shadersucsess;
    char infolog[512];
    glGetShaderiv(id, statustype, &shadersucsess);
    if (!shadersucsess)
    {

        glGetShaderInfoLog(id, 512, NULL, infolog);
        std::cout << name;
        Assert(infolog);
    }
}
int compileshader(const char* name, GLint shadertype)
{
    std::ifstream shaderstream(name);
    if (shaderstream.fail())
    {
        std::cout<<"shader filestream error"<<"\n" <<"could not find shader named " << name;
        Assert("shader name error");
    }
    std::string shadercode= std::string(std::istreambuf_iterator<char>(shaderstream), std::istreambuf_iterator<char>());
    shaderstream.close();
    const char* c_strcode= shadercode.c_str();
   
    unsigned int shaderid= glad_glCreateShader(shadertype);
    glShaderSource(shaderid, 1, &c_strcode, NULL);
    glCompileShader(shaderid);
    shaderstatuscheck(shaderid, GL_COMPILE_STATUS, name);
    return shaderid;
}
shader::shader(const char* ShaderName, const char* vertexpath, const char* fragpath)
{
    Name = ShaderName;
  int VertexShader=   compileshader(vertexpath, GL_VERTEX_SHADER);
    int FragmentShader = compileshader(fragpath, GL_FRAGMENT_SHADER);
    id= glCreateProgram();
    glAttachShader(id, VertexShader);
    glAttachShader(id, FragmentShader);
    glad_glLinkProgram(id);
    shaderstatuscheck(id, GL_LINK_STATUS, "shader program");
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);
    
}


void shader::attach()
{
    if (id==-1)
    {
        Assert("cant attach invalid shader");
    }
    glUseProgram(id);
}
