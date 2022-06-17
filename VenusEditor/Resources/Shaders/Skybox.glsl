#type vertex
#version 450 core

// In
layout(location = 0) in vec3 a_Position;

// Uniforms
layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewMatrix;
	mat4 u_ProjectionMatrix;
	mat4 u_ViewProjectionMatrix;
	mat4 u_InverseViewProjectionMatrix;
};

// Out
layout (location = 0) out vec3 v_TexCoords;

void main()
{
	v_TexCoords = vec3(a_Position.x, a_Position.y, -a_Position.z);

	mat4 rotView = mat4(mat3(u_ViewMatrix));
	vec4 clipPos = u_ProjectionMatrix * rotView * vec4(a_Position, 1.0);
	gl_Position = clipPos;
}

#type fragment
#version 450 core

// In
layout (location = 0) in vec3 v_TexCoords;

// Out
layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

// Uniforms
layout (binding = 0) uniform samplerCube u_Texture;

layout (push_constant) uniform Uniforms
{
	float TextureLod;
	float Intensity;
} u_Uniforms;

void main()
{
	o_Color = textureLod(u_Texture, v_TexCoords, u_Uniforms.TextureLod) * u_Uniforms.Intensity;
	o_EntityID = -1;
}
