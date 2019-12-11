#version 410 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
layout(location = 3) in vec3 InstanceMidPos;
layout(location = 4) in vec3 InstanceStartPos;
layout(location = 5) in vec3 InstanceEndPos;
layout(location = 6) in vec2 InstancePhases;
layout(location = 7) in vec2 InstanceTexOffset;


uniform mat4 P;
uniform mat4 V;
uniform mat4 ScaleM;
uniform int myCubeDim;
uniform int animation_stage;
uniform float interp;
out vec3 vertex_pos;
out vec2 vertex_tex;
out vec2 offset_tex;


float PI_CONST = (103993.0f / 33102.0f);

vec3 v3_linear_interpolate(vec3 a, vec3 b, float z)
{
    vec3 result;

z = clamp(z,0,1);
z= 1-z;

    result.x = a.x * z + (1 - z) * b.x;
    result.y = a.y * z + (1 - z) * b.y;
    result.z = a.z * z + (1 - z) * b.z;

    return result;
}

float map(
    float value, float min1, float max1, float min2, float max2)
{
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float transform_interp_constant(float z)
{

z = map(z, 0.0f, 1.0f, -1 * PI_CONST * 0.5, PI_CONST * 0.5f);
        z = sin(z);
        z = map(z, -1, 1, 0, 1);
		return z;
		}

void main()
{

	vec4 pos = ScaleM * vec4(vertPos,1.0);
	
	float z = clamp(interp, -1.0f, 1.0f);
	

	vec3 from, to;
	if(animation_stage == 1)
	{
		from = InstanceStartPos;
		to = InstanceMidPos;
		z += InstancePhases.x;
	}
	else if (animation_stage == 2)
	{
	from = InstanceMidPos;
		to = InstanceEndPos;
		z += InstancePhases.y;
	}
	z = clamp(z, 0.0f, 1.0f);
	z = transform_interp_constant(z);
	vec3 plus_pos = v3_linear_interpolate(from, to,z);

	pos += vec4(plus_pos, 0.0f);

	gl_Position = P * V * pos;
	vertex_tex = vertTex;	
	offset_tex = InstanceTexOffset;

	vertex_pos = pos.xyz;
}
