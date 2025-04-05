#include <glad/glad.h>
#include <glm/mat4x4.hpp>

#include "../debugger/debug.h"
#ifndef shader_hpp
#define shader_hpp

struct shader
{

	int id;

	shader(const char* vertexpath, const char* fragpath);

	shader() {
		id = -1;
	}
	int uniformlocation(const char* name) {
		if (id==-1)
		{
			Assert("invalid id");
		}
	return	glGetUniformLocation(id, name);

	}
	void setf(float val, const char* name)
	{
	
		glad_glUniform1f(uniformlocation(name), val);
	}
	void setMat4(glm::mat4 val, const char* name)
	{
	
		glad_glUniformMatrix4fv(uniformlocation(name), 1, GL_FALSE, &val[0][0]);
		
	}
	void SetVector4f(glm::vec4 val, const char* name)
	{
	
		glad_glUniform4f(uniformlocation(name), val.x, val.y, val.z, val.w);

		
	}
	void SetVector3f(glm::vec3 val, const char* name)
	{
	
		glad_glUniform3f(uniformlocation(name), val.x, val.y, val.z);

		
	}
	void SetVector2f(glm::vec2 val, const char* name)
	{
	
		glad_glUniform2f(uniformlocation(name), val.x, val.y);
	}


	void attach();
	
};
#endif

