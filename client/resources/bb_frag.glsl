#version 410 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform sampler2D title_tex;

void main()
{
vec4 tcol = texture(title_tex, vertex_tex);
color = tcol;
}
