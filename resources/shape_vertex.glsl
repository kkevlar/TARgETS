#version 330 core
layout(location = 0) in vec3 vertPos;
//layout(location = 1) in vec3 vertNor;
//layout(location = 2) in vec3 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 fragPos;
//out vec3 fragTex;
//out vec3 fragNor;

void main()
{
	//	fragNor = vec4(vec4(vertNor, 0.0) * M).xyz;
	
//	fragTex=vertTex;
	gl_Position = P * V * M * vec4(vertPos, 1.0);
	fragPos = vec3(0,0,0);
	fragPos.x = gl_Position.x;
	fragPos.y = gl_Position.y;
	fragPos.z = gl_Position.z;
}
