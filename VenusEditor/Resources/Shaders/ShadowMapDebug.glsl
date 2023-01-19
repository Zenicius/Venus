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
layout(binding = 0) uniform sampler2DArray u_Texture;
layout (push_constant) uniform Settings
{
	int Layer;
} u_Settings;

void main()
{
	float depthValue = texture(u_Texture, vec3(v_TexCoord, u_Settings.Layer)).r;

	if(u_Settings.Layer == 0)
		o_Color = vec4(depthValue, depthValue, depthValue, depthValue);
	else if(u_Settings.Layer == 1)
		o_Color = vec4(0.0, depthValue, 0.0, 1.0);
	else if(u_Settings.Layer == 2)
		o_Color = vec4(0.0, 0.0, depthValue, 1.0);
	else if(u_Settings.Layer == 3)
		o_Color = vec4(0.0, depthValue, depthValue, 1.0);
	else
		o_Color = vec4(0.0, 0.0, 0.0, 1.0);
}