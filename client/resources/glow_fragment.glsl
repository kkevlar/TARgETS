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
	
	
	color.rgba = vec4(1,0.33,0,0.33);
	
}
