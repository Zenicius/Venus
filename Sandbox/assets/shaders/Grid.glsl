#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

// Uniforms
layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewMatrix;
	mat4 u_ProjectionMatrix;
	mat4 u_ViewProjectionMatrix;
	vec3 u_Position;
};

layout(std140, binding = 2) uniform Grid
{
	mat4 u_Transform;
    	float u_Scale;
    	float u_Size;
};

layout (location = 0) out vec2 v_TexCoord;
layout (location = 1) out float v_Scale;
layout (location = 2) out float v_Size;

void main()
{
	vec4 position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);
	gl_Position = position;

	v_TexCoord = a_TexCoord;
    v_Scale = u_Scale;
    v_Size = u_Size;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;


layout (location = 0) in vec2 v_TexCoord;
layout (location = 1) in float v_Scale;
layout (location = 2) in float v_Size;

float grid(vec2 st, float res)
{
	vec2 grid = fract(st);
	return step(res, grid.x) * step(res, grid.y);
}

void main()
{
	o_EntityID = -1;

	float x = grid(v_TexCoord * v_Scale, v_Size);
	o_Color = vec4(vec3(0.2), 0.5) * (1.0 - x);

	if (o_Color.a == 0.0)
		discard;
}
