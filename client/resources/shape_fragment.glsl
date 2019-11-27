#version 330 core
out vec4 color;
in vec3 fragPos;
//in vec3 fragTex;
//in vec3 fragNor;
//uniform vec3 camPos;
uniform vec3 bonuscolor;
void main()
{
	//vec3 normal = normalize(fragNor);
	//vec3 oof = fragPos - camPos;
	//oof = normalize(oof);
	
	
	color.rgb = vec3(bonuscolor);
	color.a = 1;
	
}
