#type vertex
#version 450 core

// In
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

// Uniforms
layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewMatrix;
	mat4 u_ProjectionMatrix;
	mat4 u_ViewProjectionMatrix;
	mat4 u_InverseViewProjectionMatrix;
};

layout(std140, binding = 1) uniform Grid
{
	mat4 u_Transform;
	int u_EntityID;
};

// Out

layout (location = 0) out vec2 v_TexCoord;

void main()
{
	vec4 position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);
	gl_Position = position;

	v_TexCoord = a_TexCoord;
}

#type fragment
#version 450 core

// In
layout (location = 0) in vec2 v_TexCoord;

// Uniforms
layout (push_constant) uniform Settings
{
	layout (offset = 64) float Scale;
	float Size;
} u_Settings;

// Out
layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

float grid(vec2 st, float res)
{
	vec2 grid = fract(st);
	return step(res, grid.x) * step(res, grid.y);
}

void main()
{
	//float scale = 16.025;
	//float size = 0.025;

	float x = grid(v_TexCoord * u_Settings.Scale, u_Settings.Size);

	o_EntityID = -1;
	o_Color = vec4(vec3(0.2), 0.5) * (1.0 - x);

	if (o_Color.a == 0.0)
		discard;
}
