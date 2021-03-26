#version 430

layout (location = 0) in vec3 pos;
layout (location = 1) in float ilt;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float lt;

void main() 
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    lt = ilt;

}
