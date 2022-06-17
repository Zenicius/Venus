#pragma once

#include "glm/glm.hpp"

#include "Texture.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/MeshMaterial.h"

#include <map>

struct aiNode;
struct aiMesh;
struct aiMaterial;
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;

namespace Venus {

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
		glm::vec2 TexCoords;
	};

	class Mesh
	{
		public:
			Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, uint32_t materialIndex);

		private:
			void InitMesh();

			std::vector<Vertex> m_Vertices;
			std::vector<uint32_t> m_Indices;
			uint32_t m_MaterialIndex;
			
			Ref<VertexArray> m_VertexArray;
			Ref<VertexBuffer> m_VertexBuffer;
			Ref<IndexBuffer> m_IndexBuffer;

			friend class Renderer;
			friend class Model;
	};

	class Model
	{
		public:
			static Ref<Model> Create(const std::string& path);
			Model();
			Model(const std::string& path);

			void LogMeshStatistics(const aiScene* scene);
			std::map<uint32_t, Ref<MeshMaterial>> GetMaterials() { return m_Materials; }

		private:
			void LoadModel();

			void ProcessNode(aiNode* node, const aiScene* scene);
			Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

			std::vector<Mesh> m_Meshes;
			std::map<uint32_t, Ref<MeshMaterial>> m_Materials; // TODO: MaterialTable
			
			std::string m_Path;

			friend class Renderer;
			friend class Factory;
	};

}

