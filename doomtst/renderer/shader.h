#ifndef shader_hpp
#define shader_hpp
#include <glad/glad.h>
#include <glm/mat4x4.hpp>

#include "../util/debug.h"
struct shader
{

	int id;

	shader(const char* vertexpath, const char* fragpath);

	shader() {
		id = -1;
	}


	void setmatval(glm::mat4 val, const char* name)
	{
		int location = glGetUniformLocation(id, name);
		glad_glUniformMatrix4fv(location, 1, GL_FALSE, &val[0][0]);
		//set val only works 
	}
	void SetVector4f(glm::vec4 val, const char* name)
	{
		int location = glGetUniformLocation(id, name);
		glad_glUniform4f(location, val.x, val.y, val.z, val.w);

		//set val only works 
	}
	void SetVector3f(glm::vec3 val, const char* name)
	{
		int location = glGetUniformLocation(id, name);
		glad_glUniform3f(location, val.x, val.y, val.z);

		//set val only works 
	}
	void SetVector2f(glm::vec2 val, const char* name)
	{
		int location = glGetUniformLocation(id, name);
		glad_glUniform2f(location, val.x, val.y);

		//set val only works 
	}


	void attach();
	
};



#endif // !shader_hpp


