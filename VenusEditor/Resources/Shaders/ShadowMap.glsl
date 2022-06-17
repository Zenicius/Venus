// Shadow Map shader

#type vertex
#version 450 core

// In
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;

layout(std140, binding = 1) uniform Model
{
	mat4 u_Transform;
	int u_EntityID;
};

void main()
{
	gl_Position = u_Transform * vec4(a_Position, 1.0);
}


// ---------------------------------------------------------------------------------


#type fragment
#version 450 core

void main()
{
}


// ---------------------------------------------------------------------------------


#type geometry
#version 450 core

layout(triangles, invocations = 4) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140, binding = 2) uniform ShadowData
{
	mat4 u_ViewProjectionMatrix[4];
};

void main()
{
	for (int i = 0; i < gl_in.length(); ++i)
	{
		gl_Layer = gl_InvocationID;
		gl_Position = u_ViewProjectionMatrix[gl_InvocationID] * gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}