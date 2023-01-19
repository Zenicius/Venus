// RENDERER 3D PBR Dev Shader

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
layout (push_constant) uniform ViewportSize
{
	int Width;
	int Height;
} u_ViewportSize;

void main()
{	
	vec2 texCoordOffset = vec2(1.0/u_ViewportSize.Width, 1.0/u_ViewportSize.Height);

	float R_fxaaSpanMax = 8.0;
	float R_fxaaReduceMin = 1.0/128.0;
	float R_fxaaReduceMul = 1.0/8.0;
	
	vec3 luma = vec3(0.299, 0.587, 0.114);	
	float lumaTL = dot(luma, texture(u_Texture, v_TexCoord.xy + (vec2(-1.0, -1.0) * texCoordOffset)).xyz);
	float lumaTR = dot(luma, texture(u_Texture, v_TexCoord.xy + (vec2(1.0, -1.0) * texCoordOffset)).xyz);
	float lumaBL = dot(luma, texture(u_Texture, v_TexCoord.xy + (vec2(-1.0, 1.0) * texCoordOffset)).xyz);
	float lumaBR = dot(luma, texture(u_Texture, v_TexCoord.xy + (vec2(1.0, 1.0) * texCoordOffset)).xyz);
	float lumaM  = dot(luma, texture(u_Texture, v_TexCoord.xy).xyz);

	vec2 dir;
	dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
	dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));
	
	float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (R_fxaaReduceMul * 0.25), R_fxaaReduceMin);
	float inverseDirAdjustment = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
	
	dir = min(vec2(R_fxaaSpanMax, R_fxaaSpanMax), 
		max(vec2(-R_fxaaSpanMax, -R_fxaaSpanMax), dir * inverseDirAdjustment)) * texCoordOffset;

	vec3 result1 = (1.0/2.0) * (
		texture(u_Texture, v_TexCoord.xy + (dir * vec2(1.0/3.0 - 0.5))).xyz +
		texture(u_Texture, v_TexCoord.xy + (dir * vec2(2.0/3.0 - 0.5))).xyz);

	vec3 result2 = result1 * (1.0/2.0) + (1.0/4.0) * (
		texture(u_Texture, v_TexCoord.xy + (dir * vec2(0.0/3.0 - 0.5))).xyz +
		texture(u_Texture, v_TexCoord.xy + (dir * vec2(3.0/3.0 - 0.5))).xyz);

	float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
	float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
	float lumaResult2 = dot(luma, result2);
	
	if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
		o_Color = vec4(result1, 1.0);
	else
		o_Color = vec4(result2, 1.0);
}