#type vertex
#version 450 core

// In
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewMatrix;
	mat4 u_ProjectionMatrix;
	mat4 u_ViewProjectionMatrix;
	vec3 u_Position;
};

// Out
struct VertexOutput
{
	vec3 nearPoint;
	vec3 farPoint;
	mat4 viewMatrix;
	mat4 projMatrix;
};

layout (location = 0) out VertexOutput Output;
//layout (location = 5) out flat int v_EntityID;

vec3 gridPlane[6] = vec3[](
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
	Output.nearPoint = UnprojectPoint(a_Position.x, a_Position.y, 0.0, u_ViewMatrix, u_ViewProjectionMatrix);
	Output.farPoint = UnprojectPoint(a_Position.x, a_Position.y, 1.0, u_ViewMatrix, u_ViewProjectionMatrix);
	Output.viewMatrix = u_ViewMatrix;
	Output.projMatrix = u_ProjectionMatrix;

	//v_EntityID = a_EntityID;

	gl_Position = vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

// In
struct VertexOutput
{
	vec3 nearPoint;
	vec3 farPoint;
	mat4 viewMatrix;
	mat4 projMatrix;
};

layout (location = 0) in VertexOutput Output;
//layout (location = 5) in flat int v_EntityID;

// Out
layout (location = 0) out vec4 o_Color;
//layout (location = 1) out int o_EntityID;

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    // z axis
    if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
        color.z = 1.0;
    // x axis
    if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
        color.x = 1.0;
    return color;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = Output.projMatrix * Output.viewMatrix * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = Output.projMatrix * Output.viewMatrix * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * 0.1f * 1000.0f) / (1000.0f + 0.1f - clip_space_depth * (1000.0f - 0.1f)); // get linear value between 0.01 and 100
    return linearDepth / 1000.0f; // normalize
}

void main()
{
	float t = -Output.nearPoint.y / (Output.farPoint.y - Output.nearPoint.y);
    vec3 fragPos3D = Output.nearPoint + t * (Output.farPoint - Output.nearPoint);

	gl_FragDepth = computeDepth(fragPos3D);

	float linearDepth = computeLinearDepth(fragPos3D);
	float fading = max(0, (0.5 - linearDepth));

	o_Color = (grid(fragPos3D, 10, true) + grid(fragPos3D, 1, true))* float(t > 0);
	o_Color.a *= fading;

	//o_EntityID = v_EntityID;
}
