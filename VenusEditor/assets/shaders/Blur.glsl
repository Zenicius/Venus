// RENDERER 3D Blur Shader

#type vertex
#version 450 core

// In
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

// Out
layout (location = 0) out vec2 TexCoords;
layout (location = 4) out flat int v_EntityID;

void main()
{	
	TexCoords = a_TexCoord;
	v_EntityID = -1;

	gl_Position = vec4(a_Position.x, a_Position.y, 0.0, 1.0);
}

#type fragment
#version 450 core

layout (location = 0) in vec2 TexCoords;
layout (location = 4) in flat int v_EntityID;

// Out
layout (location = 0) out vec4 o_Color;
layout (location = 1) out int o_EntityID;

// Uniforms
layout (binding = 0) uniform sampler2D u_Textures[32];

const float offset = 1.0 / 300.0;

vec2 offsets[9] = vec2[] (
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right
);

float kernel[9] = float[] (
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16
);

void main()
{
	vec4 sampleTex[9];
	vec4 col = vec4(0.0, 0.0, 0.0, 1.0);
	
	for(int i = 0; i < 9; i++)
	{
		sampleTex[i] = vec4(texture(u_Textures[0], TexCoords.st + offsets[i]));
	}

	for(int i = 0; i < 9; i++)
	{
		col += sampleTex[i] * kernel[i];
	}

	o_Color = col;
	o_EntityID = v_EntityID;
}