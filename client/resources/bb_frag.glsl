#version 410 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform sampler2D title_tex;
uniform sampler2D normal_map_tex;

uniform vec3 campos;
uniform vec3 light1pos;
//uniform vec3 light2pos;


void main()
{

vec2 vt = vertex_tex;
vt.y = 1- vt.y;

vec3 normal = normalize(texture(normal_map_tex, vt).rgb);
vec4 tcol = texture(title_tex, vt);


vec3 camdir = normalize(vertex_pos - campos);
vec3 light1dir = normalize(vertex_pos - light1pos  );
//vec3 light2dir = normalize(vertex_pos - light2pos  );

float diffuse_factor1 = dot(normal,light1dir );
diffuse_factor1 = clamp(diffuse_factor1,0.0f,1.0f) * 0.5f;

//float diffuse_factor2 = dot(normal,light2dir );
//diffuse_factor2 = clamp(diffuse_factor2,0.0f,1.0f);

vec3 halfangle = normalize(camdir + light1dir);

float spec_factor1 = dot(normal, halfangle);
spec_factor1 = clamp(spec_factor1,0.0f,1.0f);
spec_factor1 = pow(spec_factor1, 20) * 5;

//float spec_factor2 = dot(light2dir, halfangle);
//spec_factor2 = clamp(spec_factor2,0,1);
//spec_factor2 = pow(spec_factor2, 40);


float ambient_factor = 0.01;


tcol.rgb *= (diffuse_factor1 + ambient_factor);
tcol += vec3(1,1,1) * spec_factor1;


color = tcol;

}
