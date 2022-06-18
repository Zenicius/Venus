#type vertex
#version 450 core

// In
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

// Out
layout (location = 0) out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = vec4(a_Position.x, a_Position.y, 0.0, 1.0);
}

#type fragment
#version 450 core

// In
layout(location = 0) in vec2 v_TexCoord;

// Out
layout(location = 0) out vec4 o_Color;

// Uniforms
layout(binding = 0) uniform sampler2D u_Texture;
layout (push_constant) uniform Settings
{
	float Layer;
} u_Settings;

void main()
{
	vec4 color = textureLod(u_Texture, v_TexCoord, u_Settings.Layer);
	o_Color = color;
}