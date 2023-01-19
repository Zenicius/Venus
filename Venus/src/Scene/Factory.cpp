#include "pch.h"
#include "Factory.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Venus {

	Ref<Model> Factory::CreateCube(const glm::vec3& size)
	{
		std::vector<Vertex> vertices;
		vertices.resize(8);
		vertices[0].Position = { -size.x / 2.0f, -size.y / 2.0f,  size.z / 2.0f };
		vertices[1].Position = {  size.x / 2.0f, -size.y / 2.0f,  size.z / 2.0f };
		vertices[2].Position = {  size.x / 2.0f,  size.y / 2.0f,  size.z / 2.0f };
		vertices[3].Position = { -size.x / 2.0f,  size.y / 2.0f,  size.z / 2.0f };
		vertices[4].Position = { -size.x / 2.0f, -size.y / 2.0f, -size.z / 2.0f };
		vertices[5].Position = {  size.x / 2.0f, -size.y / 2.0f, -size.z / 2.0f };
		vertices[6].Position = {  size.x / 2.0f,  size.y / 2.0f, -size.z / 2.0f };
		vertices[7].Position = { -size.x / 2.0f,  size.y / 2.0f, -size.z / 2.0f };

		vertices[0].Normal = { -1.0f, -1.0f,  1.0f };
		vertices[1].Normal = {  1.0f, -1.0f,  1.0f };
		vertices[2].Normal = {  1.0f,  1.0f,  1.0f };
		vertices[3].Normal = { -1.0f,  1.0f,  1.0f };
		vertices[4].Normal = { -1.0f, -1.0f, -1.0f };
		vertices[5].Normal = {  1.0f, -1.0f, -1.0f };
		vertices[6].Normal = {  1.0f,  1.0f, -1.0f };
		vertices[7].Normal = { -1.0f,  1.0f, -1.0f };

		std::vector<uint32_t> indices;
		indices.resize(36);
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		indices[3] = 2;
		indices[4] = 3;
		indices[5] = 0;

		indices[6] = 1;
		indices[7] = 5;
		indices[8] = 6;

		indices[9] = 6;
		indices[10] = 2;
		indices[11] = 1;

		indices[12] = 7;
		indices[13] = 6;
		indices[14] = 5;

		indices[15] = 5;
		indices[16] = 4;
		indices[17] = 7;

		indices[18] = 4;
		indices[19] = 0;
		indices[20] = 3;

		indices[21] = 3;
		indices[22] = 7;
		indices[23] = 4;

		indices[24] = 4;
		indices[25] = 5;
		indices[26] = 1;

		indices[27] = 1;
		indices[28] = 0;
		indices[29] = 4;

		indices[30] = 3;
		indices[31] = 2;
		indices[32] = 6;

		indices[33] = 6;
		indices[34] = 7;
		indices[35] = 3;

		Ref<MeshMaterial> material = CreateRef<MeshMaterial>();

		Mesh mesh(vertices, indices, 0);
		Ref<Model> cube = CreateRef<Model>();
		cube->GetMaterialTable()->SetMaterial(0, material);
		cube->m_Meshes.push_back(mesh);
		
		return cube;
	}

	Ref<Model> Factory::CreateSphere(float radius)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		constexpr float latitudeBands = 30;
		constexpr float longitudeBands = 30;

		for (float latitude = 0.0f; latitude <= latitudeBands; latitude++)
		{
			const float theta = latitude * (float)M_PI / latitudeBands;
			const float sinTheta = glm::sin(theta);
			const float cosTheta = glm::cos(theta);

			for (float longitude = 0.0f; longitude <= longitudeBands; longitude++)
			{
				const float phi = longitude * 2.f * (float)M_PI / longitudeBands;
				const float sinPhi = glm::sin(phi);
				const float cosPhi = glm::cos(phi);

				Vertex vertex;
				vertex.Normal = { cosPhi * sinTheta, cosTheta, sinPhi * sinTheta };
				vertex.Position = { radius * vertex.Normal.x, radius * vertex.Normal.y, radius * vertex.Normal.z };
				vertices.push_back(vertex);
			}
		}

		for (uint32_t latitude = 0; latitude < (uint32_t)latitudeBands; latitude++)
		{
			for (uint32_t longitude = 0; longitude < (uint32_t)longitudeBands; longitude++)
			{
				const uint32_t first = (latitude * ((uint32_t)longitudeBands + 1)) + longitude;
				const uint32_t second = first + (uint32_t)longitudeBands + 1;

				indices.push_back(first);
				indices.push_back(second);
				indices.push_back(first + 1);
				indices.push_back(second);
				indices.push_back(second + 1);
				indices.push_back(first + 1);
			}
		}

		Ref<MeshMaterial> material = CreateRef<MeshMaterial>();

		Mesh mesh(vertices, indices, 0);
		Ref<Model> sphere = CreateRef<Model>();
		sphere->GetMaterialTable()->SetMaterial(0, material);
		sphere->m_Meshes.push_back(mesh);

		return sphere;
	}

}