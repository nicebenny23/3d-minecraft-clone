#version 330 core
layout (location=0) in vec2 aPos;
layout (location=1) in vec3 texcord;
uniform float aspectratio;

    out vec3 ourcoord;


    void main()
    {
    vec4 pos=vec4(aPos.x,aPos.y,1.0f,1.0f);
    pos.y=pos.y*aspectratio;
    gl_Position =pos;
    ourcoord = texcord;
}