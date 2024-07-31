
#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec2 texcoord; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 offset;
uniform vec3 scale;
void main()
{
TexCoords = texcoord;
    vec4 pos = projection*view* vec4((vertex * scale) + offset, 1.0f);
    pos.y=pos.y*1.77;
    gl_Position=pos;
}