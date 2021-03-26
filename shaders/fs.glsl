#version 430

in float lt;
uniform sampler2D itex;
out vec4 ocolor;

void main () 
{
    vec4 color = vec4(1.0f, 0.25f, 0.0f, lt);
	ocolor = vec4(texture(itex, gl_PointCoord) * color);

}
