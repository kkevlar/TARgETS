#version 330 core
out vec4 color;
in vec3 vertex_color;
uniform vec3 bonuscolor;
void main()
{
	color.rgb = vertex_color;
	color.rgb = bonuscolor;
	color.a=1;	//transparency: 1 .. 100% NOT transparent
}
