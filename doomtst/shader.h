#ifndef shader_hpp
#define shader_hpp
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
 
struct shader
{

	int id;

	shader(const char* vertexpath, const char* fragpath);

	shader() {
		id = -1;
	}

	void setvalmat(glm::mat4 val, const char* name)
	{
		int location = glGetUniformLocation(id, name);
		glad_glUniformMatrix4fv(location, 1, GL_FALSE, &val[0][0]);
//set val only works 
	}
	template<typename T>
	void setval(T val, const char* name) {
		int location = glGetUniformLocation(id, name);

		const char* type = typeid(T).name();
		if (location ==-1)
		{
			return;
		}
		if (type == "int")
		{
			return;
		}
	
	
	
	
	}

	void attach();
	
};



#endif // !shader_hpp


