
#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 position, vec2 texCoords>
layout (location = 0) in vec2 texcoord; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 offset;

void main()
{
    float scale = .10f;
    TexCoords = texcoord;
    gl_Position = projection*view * vec4((vertex * scale) + offset, 1.0);
}