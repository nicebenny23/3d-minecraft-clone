
#pragma once
#include <string>
#include <stdexcept>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include "../util/vector2.h"
#include "../util/vector3.h"
namespace Shaders {
	class ShaderManager;  // Forward declaration of TextureManager

}
struct shader
{

	size_t id;
	std::string Name;
	

	shader() {
		Name = "";
		id = 0;
	}
	int uniformlocation(const char* name) {
		if (id==0)
		{
			throw std::logic_error("invalid id");
		}
		GLint location = glGetUniformLocation(id, name);
		if (location == -1) {
			std::string msg = "Uniform '" + std::string(name) + "' not found or not active in shader '" + Name + "'";
			throw std::logic_error(msg);
		}
		return location;

	}
	void setf(float val, const char* name)
	{
	
		glad_glUniform1f(uniformlocation(name), val);
	}
	void setint(int val, const char* name)
	{
		int l = uniformlocation(name);
		
		glad_glUniform1i(uniformlocation(name), val);
	}
	void setuint(unsigned int val, const char* name)
	{

		glad_glUniform1ui(uniformlocation(name), val);
	}
	void setMat3(glm::mat3 val, const char* name)
	{

		glad_glUniformMatrix3fv(uniformlocation(name), 1, GL_FALSE, &val[0][0]);

	}
	void setMat4(glm::mat4 val, const char* name)
	{
	
		glad_glUniformMatrix4fv(uniformlocation(name), 1, GL_FALSE, &val[0][0]);
		
	}
	void SetVector4f(glm::vec4 val, const char* name)
	{
	
		glad_glUniform4f(uniformlocation(name), val.x, val.y, val.z, val.w);

		
	}
	void SetVector3f(v3::Vec3 val, const char* name)
	{
	
		glad_glUniform3f(uniformlocation(name), val.x, val.y, val.z);

		
	}
	void SetVector2f(v2::Vec2 val, const char* name)
	{
	
		glad_glUniform2f(uniformlocation(name), val.x, val.y);
	}

	void attach();
	shader(const std::string& name, const char* vertexpath, const char* fragpath);
private:
	friend class Shaders::ShaderManager;
};

