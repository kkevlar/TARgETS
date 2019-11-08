#version 330 core
out vec4 color;
in vec3 fragPos;
in vec3 fragTex;
in vec3 fragNor;
uniform int isglass;
uniform vec3 camPos;
void main()
{
	vec3 normal = normalize(fragNor);
	vec3 oof = fragPos - camPos;
	oof = normalize(oof);
	
	if(isglass == 1)
	{
	color.rgb = vec3(1,1,1);
	color.a= 0.3 * dot(oof,normal) + 0.3;	//transparency: 1 .. 100% NOT transparent
	}
	else
	{
	color.rgb = normal;
	color.a = 1;
	}
}
