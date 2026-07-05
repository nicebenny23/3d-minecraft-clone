#version 330 core
layout (location=0) in vec2 pos;
layout (location=1) in vec3 texcoord;
uniform vec2 scale;
uniform vec2 center;
uniform float aspectratio;

    out vec3 outcoord;


    void main()
    {
    vec4 pos = vec4(pos*scale+2*  center,1, 1.0);
    pos.y=pos.y*aspectratio;
    gl_Position =pos;
    outcoord = texcoord;
}