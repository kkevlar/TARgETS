#version 330 core
out vec4 color;
in vec3 vertex_color;
uniform vec3 bonuscolor;
void main()
{
	color.rgb = vertex_color;
	color.rgb = color.rgb * 0.5 + bonuscolor * 0.5;
	color.a=1;	//transparency: 1 .. 100% NOT transparent
}
