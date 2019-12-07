#version 410 core
out vec4 color;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform sampler2D skybox_texture;

void main()
{

vec2 vt = vertex_tex;
vt.y = 1- vt.y;


vec4 tcol = texture(skybox_texture, vt);


color = tcol;

}
