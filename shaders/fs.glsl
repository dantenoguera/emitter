#version 400

uniform sampler2D itex;
out vec4 ocolor;

void main () 
{
    vec4 color = vec4(1.0f, 0.25f, 0.0f, 1.0f);
	ocolor = vec4(texture(itex, gl_PointCoord) * color);
}
