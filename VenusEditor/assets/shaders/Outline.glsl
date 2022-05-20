// RENDERER 3D PBR Dev Shader

#type vertex
#version 450 core

// In
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;

// Uniforms
layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewMatrix;
	mat4 u_ProjectionMatrix;
	mat4 u_ViewProjectionMatrix;
	vec3 u_Position;
};

layout(std140, binding = 1) uniform Model
{
	mat4 u_Transform;
	int u_EntityID;
};

// Out
struct VertexOutput
{
	vec3 WorldPosition;
	vec3 Normal;
	vec2 TexCoord;
};

layout (location = 0) out VertexOutput Output;
layout (location = 4) out flat int v_EntityID;

void main()
{
    Output.WorldPosition = vec3(u_Transform * vec4(a_Position + a_Normal * 0.02, 1.0f));
    Output.Normal = a_Normal;
	Output.TexCoord = vec2(a_TexCoord.x, 1.0 - a_TexCoord.y);

	v_EntityID = u_EntityID;

	gl_Position = u_ViewProjectionMatrix * vec4(Output.WorldPosition, 1.0);
}

#type fragment
#version 450 core

struct VertexOutput
{
	vec3 WorldPosition;
	vec3 Normal;
	vec2 TexCoord;
};

layout(location = 4) in flat int v_EntityID;

// Out
layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

void main()
{
	o_Color = vec4(0.75, 0.18, 0.22, 1.0);
	o_EntityID = v_EntityID;
}