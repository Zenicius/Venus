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
	mat4 u_ViewProjection;
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
    Output.WorldPosition = vec3(u_Transform * vec4(a_Position, 1.0f));
    Output.Normal = a_Normal;
	Output.TexCoord = vec2(a_TexCoord.x, 1.0 - a_TexCoord.y);

	v_EntityID = u_EntityID;

	gl_Position = u_ViewProjection * vec4(Output.WorldPosition, 1.0);
}

#type fragment
#version 450 core

struct VertexOutput
{
	vec3 WorldPosition;
	vec3 Normal;
	vec2 TexCoord;
};

// In
layout(location = 0) in VertexOutput Input;
layout(location = 4) in flat int v_EntityID;

// Out
layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

// Uniforms
layout(binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	o_Color = texture(u_Textures[0], Input.TexCoord);

	o_EntityID = v_EntityID;

	//o_Color = vec4(255.0, 0.0, 0.0, 1.0);
}