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
	mat4 u_InverseViewProjectionMatrix;
};

layout(std140, binding = 1) uniform Model
{
	mat4 u_Transform;
	int u_EntityID;
};

layout(std140, binding = 2) uniform ShadowData
{
	mat4 u_LightMatrix[4];
};

// Out
struct VertexOutput
{
	vec3 WorldPosition;
	vec3 Normal;
	vec2 TexCoord;
	mat3 WorldNormals;

	vec4 ShadowMapCoords[4];
	vec3 ViewPosition;
};

layout (location = 0) out VertexOutput Output;
layout (location = 11) out flat int v_EntityID;

void main()
{
    Output.WorldPosition = vec3(u_Transform * vec4(a_Position, 1.0f));
    Output.Normal = mat3(u_Transform) * a_Normal;
	Output.TexCoord = vec2(a_TexCoord.x, 1.0 - a_TexCoord.y);
	Output.WorldNormals = mat3(u_Transform) * mat3(a_Tangent, a_Binormal, a_Normal);

	Output.ShadowMapCoords[0] = u_LightMatrix[0] * vec4(Output.WorldPosition, 1.0);
	Output.ShadowMapCoords[1] = u_LightMatrix[1] * vec4(Output.WorldPosition, 1.0);
	Output.ShadowMapCoords[2] = u_LightMatrix[2] * vec4(Output.WorldPosition, 1.0);
	Output.ShadowMapCoords[3] = u_LightMatrix[3] * vec4(Output.WorldPosition, 1.0);
	Output.ViewPosition = vec3(u_ViewMatrix * vec4(Output.WorldPosition, 1.0));

	v_EntityID = u_EntityID;
	gl_Position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

// CONSTS
const float PI = 3.141592;
const float Epsilon = 0.00001;

struct VertexOutput
{
	vec3 WorldPosition;
	vec3 Normal;
	vec2 TexCoord;
	mat3 WorldNormals;

	vec4 ShadowMapCoords[4];
	vec3 ViewPosition;
};

struct DirectionalLight
{
	vec3 Direction;
	float HasDirLight;
	vec3 Radiance;
	float Intensity;
};

struct PointLight 
{
	vec3 Position;
	float Intensity;
	vec3 Color;
	float MinRadius;
	float Radius;
	float Falloff;
	float LightSize;
	bool CastsShadows;
};

// In
layout(location = 0) in VertexOutput Input;
layout(location = 11) in flat int v_EntityID;

// Out
layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

// Uniforms
layout(std140, binding = 3) uniform SceneData
{
	DirectionalLight u_DirectionalLight;
	vec3 u_CameraPosition;
	float u_EnvironmentMapIntensity;
};

layout(std140, binding = 4) uniform PointLightData
{
	uint u_PointLightsCount;
	PointLight u_pointLights[1024];
};

layout(std140, binding = 5) uniform RendererData
{
	uniform vec4 u_CascadeSplits;
	uniform int u_TilesCountX;
	uniform bool u_ShowCascades;
	uniform bool u_SoftShadows;
	uniform float u_LightSize;
	uniform float u_MaxShadowDistance;
	uniform float u_ShadowFade;
	uniform bool u_CascadeFading;
	uniform float u_CascadeTransitionFade;
	uniform bool u_ShowLightComplexity;
};

// PBR Textures
layout(binding = 0) uniform sampler2D u_AlbedoTexture;
layout(binding = 1) uniform sampler2D u_NormalTexture;
layout(binding = 2) uniform sampler2D u_MetalnessTexture;
layout(binding = 3) uniform sampler2D u_RoughnessTexture;

// Env Map
layout(binding = 4) uniform samplerCube u_EnvRadianceTex;
layout(binding = 5) uniform samplerCube u_EnvIrradianceTex;
layout(binding = 6) uniform sampler2D u_BRDFLUTTexture;

// Shadow Map
layout(binding = 7) uniform sampler2DArray u_ShadowMapTexture;

// PBR Material Params
layout(push_constant) uniform Material
{
	vec3 AlbedoColor;
	float Metalness;
	float Roughness;
	int UseNormalMap;

} u_MaterialUniforms;

struct PBRParameters
{
	vec3 Albedo;
	float Roughness;
	float Metalness;

	vec3 Normal;
	vec3 View;
	float NdotV;
};
PBRParameters m_Params;


float ShadowFade = 1.0;

const vec2 PoissonDistribution[64] = vec2[](
	vec2(-0.884081, 0.124488),
	vec2(-0.714377, 0.027940),
	vec2(-0.747945, 0.227922),
	vec2(-0.939609, 0.243634),
	vec2(-0.985465, 0.045534),
	vec2(-0.861367, -0.136222),
	vec2(-0.881934, 0.396908),
	vec2(-0.466938, 0.014526),
	vec2(-0.558207, 0.212662),
	vec2(-0.578447, -0.095822),
	vec2(-0.740266, -0.095631),
	vec2(-0.751681, 0.472604),
	vec2(-0.553147, -0.243177),
	vec2(-0.674762, -0.330730),
	vec2(-0.402765, -0.122087),
	vec2(-0.319776, -0.312166),
	vec2(-0.413923, -0.439757),
	vec2(-0.979153, -0.201245),
	vec2(-0.865579, -0.288695),
	vec2(-0.243704, -0.186378),
	vec2(-0.294920, -0.055748),
	vec2(-0.604452, -0.544251),
	vec2(-0.418056, -0.587679),
	vec2(-0.549156, -0.415877),
	vec2(-0.238080, -0.611761),
	vec2(-0.267004, -0.459702),
	vec2(-0.100006, -0.229116),
	vec2(-0.101928, -0.380382),
	vec2(-0.681467, -0.700773),
	vec2(-0.763488, -0.543386),
	vec2(-0.549030, -0.750749),
	vec2(-0.809045, -0.408738),
	vec2(-0.388134, -0.773448),
	vec2(-0.429392, -0.894892),
	vec2(-0.131597, 0.065058),
	vec2(-0.275002, 0.102922),
	vec2(-0.106117, -0.068327),
	vec2(-0.294586, -0.891515),
	vec2(-0.629418, 0.379387),
	vec2(-0.407257, 0.339748),
	vec2(0.071650, -0.384284),
	vec2(0.022018, -0.263793),
	vec2(0.003879, -0.136073),
	vec2(-0.137533, -0.767844),
	vec2(-0.050874, -0.906068),
	vec2(0.114133, -0.070053),
	vec2(0.163314, -0.217231),
	vec2(-0.100262, -0.587992),
	vec2(-0.004942, 0.125368),
	vec2(0.035302, -0.619310),
	vec2(0.195646, -0.459022),
	vec2(0.303969, -0.346362),
	vec2(-0.678118, 0.685099),
	vec2(-0.628418, 0.507978),
	vec2(-0.508473, 0.458753),
	vec2(0.032134, -0.782030),
	vec2(0.122595, 0.280353),
	vec2(-0.043643, 0.312119),
	vec2(0.132993, 0.085170),
	vec2(-0.192106, 0.285848),
	vec2(0.183621, -0.713242),
	vec2(0.265220, -0.596716),
	vec2(-0.009628, -0.483058),
	vec2(-0.018516, 0.435703)
	);

const vec2 poissonDisk[16] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760),
	vec2(-0.91588581, 0.45771432),
	vec2(-0.81544232, -0.87912464),
	vec2(-0.38277543, 0.27676845),
	vec2(0.97484398, 0.75648379),
	vec2(0.44323325, -0.97511554),
	vec2(0.53742981, -0.47373420),
	vec2(-0.26496911, -0.41893023),
	vec2(0.79197514, 0.19090188),
	vec2(-0.24188840, 0.99706507),
	vec2(-0.81409955, 0.91437590),
	vec2(0.19984126, 0.78641367),
	vec2(0.14383161, -0.14100790)
	);

vec2 SamplePoisson(int index)
{
	return PoissonDistribution[index % 64];
}

float GetShadowBias()
{
	const float MINIMUM_SHADOW_BIAS = 0.002;
	float bias = max(MINIMUM_SHADOW_BIAS * (1.0 - dot(m_Params.Normal, u_DirectionalLight.Direction)), MINIMUM_SHADOW_BIAS);
	return bias;
}

float HardShadows_DirectionalLight(sampler2DArray shadowMap, uint cascade, vec3 shadowCoords)
{
	float bias = GetShadowBias();
	vec3 projCoords = shadowCoords * 0.5 + 0.5;
	
	float shadowMapDepth = texture(shadowMap, vec3(shadowCoords.xy * 0.5 + 0.5, cascade)).r;
	return step(projCoords.z - bias, shadowMapDepth) * ShadowFade;
}

float SearchWidth(float uvLightSize, float receiverDistance)
{
	const float NEAR = 0.1;
	return uvLightSize * (receiverDistance - NEAR) / u_CameraPosition.z;
}

float SearchRegionRadiusUV(float zWorld)
{
	const float light_zNear = 0.0; // 0.01 gives artifacts? maybe because of ortho proj?
	const float lightRadiusUV = 0.05;
	return lightRadiusUV * (zWorld - light_zNear) / zWorld;
}

float FindBlockerDistance_DirectionalLight(sampler2DArray shadowMap, uint cascade, vec3 shadowCoords, float uvLightSize)
{
	float bias = GetShadowBias();

	int numBlockerSearchSamples = 64;
	int blockers = 0;
	float avgBlockerDistance = 0;

	vec3 projCoords = shadowCoords * 0.5 + 0.5;
	//float searchWidth = SearchRegionRadiusUV(shadowCoords.z);
	float searchWidth = SearchRegionRadiusUV(projCoords.z);
	for (int i = 0; i < numBlockerSearchSamples; i++)
	{
		//float z = textureLod(shadowMap, vec3((shadowCoords.xy * 0.5 + 0.5) + SamplePoisson(i) * searchWidth, cascade), 0).r;
		float z = texture(shadowMap, vec3((shadowCoords.xy * 0.5 + 0.5) + SamplePoisson(i) * searchWidth, cascade)).r;
		if (z < (projCoords.z - bias)) //shadowCoords.z 
		{
			blockers++;
			avgBlockerDistance += z;
		}
	}

	if (blockers > 0)
		return avgBlockerDistance / float(blockers);

	return -1;
}

float PCF_DirectionalLight(sampler2DArray shadowMap, uint cascade, vec3 shadowCoords, float uvRadius)
{
	float bias = GetShadowBias();
	int numPCFSamples = 64;

	float sum = 0;
	for (int i = 0; i < numPCFSamples; i++)
	{
		vec2 offset = SamplePoisson(i) * uvRadius;
		//float z = textureLod(shadowMap, vec3((shadowCoords.xy * 0.5 + 0.5) + offset, cascade), 0).r;
		vec3 projCoords = shadowCoords * 0.5 + 0.5;
		float z = texture(shadowMap, vec3((shadowCoords.xy * 0.5 + 0.5) + offset, cascade)).r;
		sum += step(projCoords.z - bias, z);
	}
	return sum / numPCFSamples;
}

float PCSS_DirectionalLight(sampler2DArray shadowMap, uint cascade, vec3 shadowCoords, float uvLightSize)
{
	float blockerDistance = FindBlockerDistance_DirectionalLight(shadowMap, cascade, shadowCoords, uvLightSize);
	if (blockerDistance == -1) // No occlusion
		return 1.0f;

	vec3 projCoords = shadowCoords * 0.5 + 0.5;
	float penumbraWidth = (projCoords.z - blockerDistance) / blockerDistance;

	float NEAR = 0.01; // Should this value be tweakable?
	//float uvRadius = penumbraWidth * uvLightSize * NEAR / shadowCoords.z; // Do we need to divide by shadowCoords.z?
	float uvRadius = penumbraWidth * uvLightSize * NEAR;
	uvRadius = min(uvRadius, 0.002f);
	return PCF_DirectionalLight(shadowMap, cascade, shadowCoords, uvRadius) * ShadowFade;
}

//----------------------------------------------------------------------------------------------
vec3 RotateVectorAboutY(float angle, vec3 vec)
{
	angle = radians(angle);
	mat3x3 rotationMatrix = { vec3(cos(angle),0.0,sin(angle)),
							vec3(0.0,1.0,0.0),
							vec3(-sin(angle),0.0,cos(angle)) };
	return rotationMatrix * vec;
}
//----------------------------------------------------------------------------------------------

float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

float gaSchlickGGX(float cosLi, float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(NdotV, k);
}

float ndfGGX(float cosLh, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

vec3 fresnelSchlickRoughness(vec3 F0, float cosTheta, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

//----------------------------------------------------------------------------------------------

vec3 CalculateDirLight(vec3 F0)
{
	vec3 result = vec3(0.0);

	if(u_DirectionalLight.HasDirLight == 1)
	{
		vec3 Li = u_DirectionalLight.Direction;
		vec3 Lradiance = u_DirectionalLight.Radiance * u_DirectionalLight.Intensity;
		vec3 Lh = normalize(Li + m_Params.View);

		// Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(m_Params.Normal, Li));
		float cosLh = max(0.0, dot(m_Params.Normal, Lh));
		vec3 F = fresnelSchlickRoughness(F0, max(0.0, dot(Lh, m_Params.View)), m_Params.Roughness);
		float D = ndfGGX(cosLh, m_Params.Roughness);
		float G = gaSchlickGGX(cosLi, m_Params.NdotV, m_Params.Roughness);
		vec3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
		vec3 diffuseBRDF = kd * m_Params.Albedo;

		// Cook-Torrance
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * m_Params.NdotV);
		specularBRDF = clamp(specularBRDF, vec3(0.0f), vec3(10.0f));
		result += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
	}

	return result;
}

vec3 CalculatePointLights(in vec3 F0)
{
	vec3 result = vec3(0.0);
	for (int i = 0; i < u_PointLightsCount; i++)
	{
		PointLight light = u_pointLights[i];
		vec3 Li = normalize(light.Position - Input.WorldPosition);
		float lightDistance = length(light.Position - Input.WorldPosition);
		vec3 Lh = normalize(Li + m_Params.View);

		float attenuation = clamp(1.0 - (lightDistance * lightDistance) / (light.Radius * light.Radius), 0.0, 1.0);
		attenuation *= mix(attenuation, 1.0, light.Falloff);

		vec3 Lradiance = light.Color * light.Intensity * attenuation;

		// Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(m_Params.Normal, Li));
		float cosLh = max(0.0, dot(m_Params.Normal, Lh));

		vec3 F = fresnelSchlickRoughness(F0, max(0.0, dot(Lh, m_Params.View)), m_Params.Roughness);
		float D = ndfGGX(cosLh, m_Params.Roughness);
		float G = gaSchlickGGX(cosLi, m_Params.NdotV, m_Params.Roughness);

		vec3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
		vec3 diffuseBRDF = kd * m_Params.Albedo;

		// Cook-Torrance
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * m_Params.NdotV);
		specularBRDF = clamp(specularBRDF, vec3(0.0f), vec3(10.0f));
		result += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
	}
	return result;
}

vec3 IBL(vec3 F0, vec3 Lr)
{
	vec3 irradiance = texture(u_EnvIrradianceTex, m_Params.Normal).rgb;
	vec3 F = fresnelSchlickRoughness(F0, m_Params.NdotV, m_Params.Roughness);
	vec3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
	vec3 diffuseIBL = m_Params.Albedo * irradiance;

	int envRadianceTexLevels = textureQueryLevels(u_EnvRadianceTex);
	float NoV = clamp(m_Params.NdotV, 0.0, 1.0);
	vec3 R = 2.0 * dot(m_Params.View, m_Params.Normal) * m_Params.Normal - m_Params.View;
	vec3 specularIrradiance = textureLod(u_EnvRadianceTex, RotateVectorAboutY(0.0f, Lr), (m_Params.Roughness) * envRadianceTexLevels).rgb;
	//specularIrradiance = vec3(Convert_sRGB_FromLinear(specularIrradiance.r), Convert_sRGB_FromLinear(specularIrradiance.g), Convert_sRGB_FromLinear(specularIrradiance.b));

	// Sample BRDF Lut, 1.0 - roughness for y-coord because texture was generated (in Sparky) for gloss model
	vec2 specularBRDF = texture(u_BRDFLUTTexture, vec2(m_Params.NdotV, 1.0 - m_Params.Roughness)).rg;
	vec3 specularIBL = specularIrradiance * (F0 * specularBRDF.x + specularBRDF.y);

	return kd * diffuseIBL + specularIBL;
}

//----------------------------------------------------------------------------------------------

float ShadowCalculation()
{
    // select cascade layer
    const uint SHADOW_MAP_CASCADE_COUNT = 4;
	uint cascadeIndex = 0;
	for (uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++)
	{
		if (Input.ViewPosition.z < u_CascadeSplits[i])
			cascadeIndex = i + 1;
	}

    vec4 fragPosLightSpace = Input.ShadowMapCoords[cascadeIndex];
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0)
    {
        return 0.0;
    }

    // calculate bias (based on depth map resolution and slope)
    const float MINIMUM_SHADOW_BIAS = 0.002;
	float bias = max(MINIMUM_SHADOW_BIAS * (1.0 - dot(m_Params.Normal, u_DirectionalLight.Direction)), MINIMUM_SHADOW_BIAS);

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(u_ShadowMapTexture, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMapTexture, vec3(projCoords.xy + vec2(x, y) * texelSize, cascadeIndex)).r;
            shadow += (currentDepth - bias) > pcfDepth ? 0.0 : 1.0;        
        }    
    }
    shadow /= 9.0;
        
    return shadow;
}

float myShadow()
{
	float shadow = 1.0;
	float shadowMapDepth = 0;
	vec3 projCoords = vec3(0, 0, 0);

	const uint SHADOW_MAP_CASCADE_COUNT = 4;
	uint cascadeIndex = 0;
	for (uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++)
	{
		if (Input.ViewPosition.z < u_CascadeSplits[i])
			cascadeIndex = i + 1;
	}

	if(cascadeIndex == 0)
	{
		vec4 lightSpacePos = Input.ShadowMapCoords[0];
		projCoords = lightSpacePos.xyz * 0.5 + 0.5;
		shadowMapDepth = texture(u_ShadowMapTexture, vec3(projCoords.xy, 0)).r;
	}
	else if(cascadeIndex == 1)
	{
		vec4 lightSpacePos = Input.ShadowMapCoords[1];
		projCoords = lightSpacePos.xyz * 0.5 + 0.5;
		shadowMapDepth = texture(u_ShadowMapTexture, vec3(projCoords.xy, 1)).r;
	}
	else if(cascadeIndex == 2)
	{
		vec4 lightSpacePos = Input.ShadowMapCoords[2];
		projCoords = lightSpacePos.xyz * 0.5 + 0.5;
		shadowMapDepth = texture(u_ShadowMapTexture, vec3(projCoords.xy, 2)).r;

	}
	else if(cascadeIndex == 3)
	{
		vec4 lightSpacePos = Input.ShadowMapCoords[3];
		projCoords = lightSpacePos.xyz * 0.5 + 0.5;
		shadowMapDepth = texture(u_ShadowMapTexture, vec3(projCoords.xy, 3)).r;
	}

	const float MINIMUM_SHADOW_BIAS = 0.002;
	float bias = max(MINIMUM_SHADOW_BIAS * (1.0 - dot(m_Params.Normal, u_DirectionalLight.Direction)), MINIMUM_SHADOW_BIAS);

	float currentDepth = projCoords.z;
	shadow = currentDepth - bias > shadowMapDepth ? 0.25 : 1.0;
	return shadow;
}

vec4 GetCascadeColor()
{
	const uint SHADOW_MAP_CASCADE_COUNT = 4;
	uint cascadeIndex = 0;
	for (uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++)
	{
		if (Input.ViewPosition.z < u_CascadeSplits[i])
			cascadeIndex = i + 1;
	}

	if(cascadeIndex == 0)
	{
		return vec4(1.0, 0.0, 0.0, 1.0);
	}
	else if(cascadeIndex == 1)
	{
		return vec4(0.0, 1.0, 0.0, 1.0);
	}
	else if(cascadeIndex == 2)
	{
		return vec4(0.0, 0.0, 1.0, 1.0);
	}
	else if(cascadeIndex == 3)
	{
		return vec4(0.0, 1.0, 1.0, 1.0);
	}

	return vec4(1.0, 1.0, 1.0, 1.0);
}


void main()
{

	// --
	float gamma = 2.2;
	//--

	// Set PBR Params
	m_Params.Albedo = texture(u_AlbedoTexture, Input.TexCoord).rgb * u_MaterialUniforms.AlbedoColor;
	float alpha = texture(u_AlbedoTexture, Input.TexCoord).a;
	m_Params.Metalness = texture(u_MetalnessTexture, Input.TexCoord).r * u_MaterialUniforms.Metalness;
	m_Params.Roughness = texture(u_RoughnessTexture, Input.TexCoord).r * u_MaterialUniforms.Roughness;
	m_Params.Roughness = max(m_Params.Roughness, 0.05); // Minimum roughness of 0.05 to keep specular highlight

	// Normals 
	m_Params.Normal = normalize(Input.Normal);
	if (u_MaterialUniforms.UseNormalMap == 1)
	{
		m_Params.Normal = normalize(texture(u_NormalTexture, Input.TexCoord).rgb * 2.0f - 1.0f);
		m_Params.Normal = normalize(Input.WorldNormals * m_Params.Normal);
	}
	m_Params.View = normalize(u_CameraPosition - Input.WorldPosition);
	m_Params.NdotV = max(dot(m_Params.Normal, m_Params.View), 0.0);

	// Specular reflection vector
	vec3 Lr = 2.0 * m_Params.NdotV * m_Params.Normal - m_Params.View;

	// Fresnel reflectance, metals use albedo
	vec3 Fdielectric = vec3(0.04);
	vec3 F0 = mix(Fdielectric, m_Params.Albedo, m_Params.Metalness);

	// Shadows
	uint cascadeIndex = 0;
	const uint SHADOW_MAP_CASCADE_COUNT = 4;
	for (uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++)
	{
		if (Input.ViewPosition.z < u_CascadeSplits[i])
			cascadeIndex = i + 1;
	}

	float shadowDistance = u_MaxShadowDistance;//u_CascadeSplits[3];
	float transitionDistance = u_ShadowFade;
	float distance = length(Input.ViewPosition);
	ShadowFade = distance - (shadowDistance - transitionDistance);
	ShadowFade /= transitionDistance;
	ShadowFade = clamp(1.0 - ShadowFade, 0.0, 1.0);
	float shadowAmount = 1.0;

	bool fadeCascades = u_CascadeFading;
	if (fadeCascades)
	{
		float cascadeTransitionFade = u_CascadeTransitionFade;

		float c0 = smoothstep(u_CascadeSplits[0] + cascadeTransitionFade * 0.5f, u_CascadeSplits[0] - cascadeTransitionFade * 0.5f, Input.ViewPosition.z);
		float c1 = smoothstep(u_CascadeSplits[1] + cascadeTransitionFade * 0.5f, u_CascadeSplits[1] - cascadeTransitionFade * 0.5f, Input.ViewPosition.z);
		float c2 = smoothstep(u_CascadeSplits[2] + cascadeTransitionFade * 0.5f, u_CascadeSplits[2] - cascadeTransitionFade * 0.5f, Input.ViewPosition.z);
		if (c0 > 0.0 && c0 < 1.0)
		{
			// Sample 0 & 1
			vec3 shadowMapCoords = (Input.ShadowMapCoords[0].xyz / Input.ShadowMapCoords[0].w);
			float shadowAmount0 = u_SoftShadows ? PCSS_DirectionalLight(u_ShadowMapTexture, 0, shadowMapCoords, u_LightSize) : HardShadows_DirectionalLight(u_ShadowMapTexture, 0, shadowMapCoords);
			shadowMapCoords = (Input.ShadowMapCoords[1].xyz / Input.ShadowMapCoords[1].w);
			float shadowAmount1 = u_SoftShadows ? PCSS_DirectionalLight(u_ShadowMapTexture, 1, shadowMapCoords, u_LightSize) : HardShadows_DirectionalLight(u_ShadowMapTexture, 1, shadowMapCoords);

			shadowAmount = mix(shadowAmount0, shadowAmount1, c0);
		}
		else if (c1 > 0.0 && c1 < 1.0)
		{
			// Sample 1 & 2
			vec3 shadowMapCoords = (Input.ShadowMapCoords[1].xyz / Input.ShadowMapCoords[1].w);
			float shadowAmount1 = u_SoftShadows ? PCSS_DirectionalLight(u_ShadowMapTexture, 1, shadowMapCoords, u_LightSize) : HardShadows_DirectionalLight(u_ShadowMapTexture, 1, shadowMapCoords);
			shadowMapCoords = (Input.ShadowMapCoords[2].xyz / Input.ShadowMapCoords[2].w);
			float shadowAmount2 = u_SoftShadows ? PCSS_DirectionalLight(u_ShadowMapTexture, 2, shadowMapCoords, u_LightSize) : HardShadows_DirectionalLight(u_ShadowMapTexture, 2, shadowMapCoords);

			shadowAmount = mix(shadowAmount1, shadowAmount2, c1);
		}
		else if (c2 > 0.0 && c2 < 1.0)
		{
			// Sample 2 & 3
			vec3 shadowMapCoords = (Input.ShadowMapCoords[2].xyz / Input.ShadowMapCoords[2].w);
			float shadowAmount2 = u_SoftShadows ? PCSS_DirectionalLight(u_ShadowMapTexture, 2, shadowMapCoords, u_LightSize) : HardShadows_DirectionalLight(u_ShadowMapTexture, 2, shadowMapCoords);
			shadowMapCoords = (Input.ShadowMapCoords[3].xyz / Input.ShadowMapCoords[3].w);
			float shadowAmount3 = u_SoftShadows ? PCSS_DirectionalLight(u_ShadowMapTexture, 3, shadowMapCoords, u_LightSize) : HardShadows_DirectionalLight(u_ShadowMapTexture, 3, shadowMapCoords);

			shadowAmount = mix(shadowAmount2, shadowAmount3, c2);
		}
		else
		{
			vec3 shadowMapCoords = (Input.ShadowMapCoords[cascadeIndex].xyz / Input.ShadowMapCoords[cascadeIndex].w);
			shadowAmount = u_SoftShadows ? PCSS_DirectionalLight(u_ShadowMapTexture, cascadeIndex, shadowMapCoords, u_LightSize) : HardShadows_DirectionalLight(u_ShadowMapTexture, cascadeIndex, shadowMapCoords);
		}
	}
	else
	{
		vec3 shadowMapCoords = (Input.ShadowMapCoords[cascadeIndex].xyz / Input.ShadowMapCoords[cascadeIndex].w);
		shadowAmount = u_SoftShadows ? PCSS_DirectionalLight(u_ShadowMapTexture, cascadeIndex, shadowMapCoords, u_LightSize) : HardShadows_DirectionalLight(u_ShadowMapTexture, cascadeIndex, shadowMapCoords);
	}

	//float shadow = myShadow();
	//float shadow2 = ShadowCalculation();
	
	vec3 lightContribution = CalculateDirLight(F0) * shadowAmount;
	lightContribution += CalculatePointLights(F0);
	
	vec3 iblContribution = IBL(F0, Lr) * u_EnvironmentMapIntensity;

	o_Color = vec4(iblContribution + lightContribution, 1.0);

	if(u_ShowCascades)
		o_Color *= GetCascadeColor();

	o_EntityID = v_EntityID;
}