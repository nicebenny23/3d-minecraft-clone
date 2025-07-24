#pragma once
#include "shader.h"
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../debugger/debug.h"
#include <iostream>
//Fix
#include <vector>
#include <string>
#include <stdexcept>
void checkGLError(const char* location) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error at " << location << ": 0x" << std::hex << err << std::dec << "\n";
    }
}
void shaderstatuscheck(GLuint id, GLenum statustype, const char* name) {
    GLint success = 0;

    // Use program calls for LINK and VALIDATE status, shader calls for COMPILE status
    if (statustype == GL_LINK_STATUS || statustype == GL_VALIDATE_STATUS) {
        glGetProgramiv(id, statustype, &success);
    }
    else if (statustype == GL_COMPILE_STATUS) {
        glGetShaderiv(id, statustype, &success);
    }
    else {
        throw std::logic_error("Unsupported statustype passed to shaderstatuscheck");
    }

    
        GLint logLength = 0;
        if (statustype == GL_LINK_STATUS || statustype == GL_VALIDATE_STATUS) {
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
        }
        else { // compile status
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
        }

        if (logLength > 0) {
            std::vector<char> infoLog(logLength);
            if (statustype == GL_LINK_STATUS || statustype == GL_VALIDATE_STATUS) {
                glGetProgramInfoLog(id, logLength, nullptr, infoLog.data());
            }
            else { // compile status
                glGetShaderInfoLog(id, logLength, nullptr, infoLog.data());
            }

            infoLog.back() = '\0'; // null terminate safely

            std::string errorType;
            switch (statustype) {
            case GL_COMPILE_STATUS: errorType = "Compile error"; break;
            case GL_LINK_STATUS:    errorType = "Link error"; break;
            case GL_VALIDATE_STATUS:errorType = "Validation error"; break;
            default:                errorType = "Unknown error"; break;
            }

            std::string errorMsg = std::string(name) + " " + errorType + ": " + infoLog.data();
            if (!success)
            {

                throw std::logic_error(errorMsg);
            }
            else {
                debug( errorMsg);
            }
        }
        else if (!success){
            {
                throw std::logic_error(std::string(name) + " error: Unknown error (empty info log)");
            }
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
    checkGLError("pls");
    unsigned int shaderid= glad_glCreateShader(shadertype);
    glShaderSource(shaderid, 1, &c_strcode, NULL);
    glCompileShader(shaderid);
    shaderstatuscheck(shaderid, GL_COMPILE_STATUS, name);
    checkGLError("idk");
    return shaderid;
}
shader::shader(const std::string& ShaderName, const char* vertexpath, const char* fragpath)
{
    Name = ShaderName;
  int VertexShader=   compileshader(vertexpath, GL_VERTEX_SHADER);
    int FragmentShader = compileshader(fragpath, GL_FRAGMENT_SHADER);
    id= glCreateProgram();
    glAttachShader(id, VertexShader);
    glAttachShader(id, FragmentShader);
    glLinkProgram(id);
    shaderstatuscheck(id, GL_LINK_STATUS, "shader program"); 
  
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << std::hex << err << std::endl;

    }
    }


void shader::attach()
{
    if (id==0)
    {
        throw std::logic_error("Unable to attach invalid shader");
    } 
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << std::hex << err << std::endl;

    }
    glValidateProgram(id);
    shaderstatuscheck(id, GL_VALIDATE_STATUS, "shader program validation");
    glUseProgram(id);
}
