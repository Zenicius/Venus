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
layout(binding = 1) uniform sampler2D u_BloomTexture;
layout(binding = 2) uniform sampler2D u_BloomDirtMaskTexture;
layout(push_constant) uniform Settings
{
	float Exposure;
	int Grayscale;
	int ACESTone;
	int GammaCorrection;
	int Bloom;
	float BloomIntensity;
	float BloomDirkMaskIntensity;
} u_Settings;

vec3 UpsampleTent9(sampler2D tex, float lod, vec2 uv, vec2 texelSize, float radius)
{
	vec4 offset = texelSize.xyxy * vec4(1.0f, 1.0f, -1.0f, 0.0f) * radius;

	// Center
	vec3 result = textureLod(tex, uv, lod).rgb * 4.0f;

	result += textureLod(tex, uv - offset.xy, lod).rgb;
	result += textureLod(tex, uv - offset.wy, lod).rgb * 2.0;
	result += textureLod(tex, uv - offset.zy, lod).rgb;

	result += textureLod(tex, uv + offset.zw, lod).rgb * 2.0;
	result += textureLod(tex, uv + offset.xw, lod).rgb * 2.0;

	result += textureLod(tex, uv + offset.zy, lod).rgb;
	result += textureLod(tex, uv + offset.wy, lod).rgb * 2.0;
	result += textureLod(tex, uv + offset.xy, lod).rgb;

	return result * (1.0f / 16.0f);
}

vec3 ACESTonemap(vec3 color)
{
	mat3 m1 = mat3(
		0.59719, 0.07600, 0.02840,
		0.35458, 0.90834, 0.13383,
		0.04823, 0.01566, 0.83777
	);
	mat3 m2 = mat3(
		1.60475, -0.10208, -0.00327,
		-0.53108, 1.10813, -0.07276,
		-0.07367, -0.00605, 1.07602
	);
	vec3 v = m1 * color;
	vec3 a = v * (v + 0.0245786) - 0.000090537;
	vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
	return clamp(m2 * (a / b), 0.0, 1.0);
}

vec3 GammaCorrect(vec3 col, float gamma)
{
	return pow(col.rgb, vec3(1.0/gamma));
}

void main()
{
	const float gamma = 2.2;
	float sampleScale = 0.5;

	ivec2 texSize = textureSize(u_BloomTexture, 0);
	vec2 fTexSize = vec2(float(texSize.x), float(texSize.y));
	vec3 bloom = UpsampleTent9(u_BloomTexture, 0, v_TexCoord, 1.0f / fTexSize, sampleScale) * u_Settings.BloomIntensity;
	vec3 bloomMask = texture(u_BloomDirtMaskTexture, v_TexCoord).rgb * u_Settings.BloomDirkMaskIntensity;

	vec3 col = texture(u_Texture, v_TexCoord).rgb;
	
	if(u_Settings.Bloom == 1)
	{
		col += bloom;
		col += bloom * bloomMask;
	}

	col *= u_Settings.Exposure;

	if(u_Settings.ACESTone == 1)
		col = ACESTonemap(col);

	if(u_Settings.GammaCorrection == 1)
		col = GammaCorrect(col, gamma);

	float grayscale = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;	
	if(u_Settings.Grayscale == 1)
		col = vec3(grayscale);

	o_Color = vec4(col, 1.0);
}