#pragma once

#include "glm/glm.hpp"

#include "Texture.h"
#include <Renderer/Buffer.h>
#include <Renderer/VertexArray.h>

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

	// TEMP Move To Material 
	enum class TextureType
	{
		Diffuse = 0,
		Normal = 1,
		Specular = 2,

		Unknown = 3
	};

	struct MaterialTexture {
		uint32_t Index;
		TextureType Type;
	};

	class Mesh
	{
		public:
			Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<MaterialTexture> textures);

		private:
			void InitMesh();

			std::vector<Vertex> m_Vertices;
			std::vector<uint32_t> m_Indices;
			
			Ref<VertexArray> m_VertexArray;
			Ref<VertexBuffer> m_VertexBuffer;
			Ref<IndexBuffer> m_IndexBuffer;

			std::vector<MaterialTexture> m_Textures;

			friend class Renderer;
			friend class Model;
	};

	class Model
	{

		public:
			Model();
			Model(const std::string& path);

			void LogMeshStatistics();

		private:
			void LoadModel();
			void Init();

			void ProcessNode(aiNode* node, const aiScene* scene);
			Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

			std::vector<MaterialTexture> LoadMaterialTextures(aiMaterial* material, TextureType type);

			std::vector<Mesh> m_Meshes;
			std::vector<Ref<Texture2D>> m_DiffuseMaps;
			std::vector<Ref<Texture2D>> m_NormalMaps;
			std::vector<Ref<Texture2D>> m_SpecularMaps;

			std::string m_Path;

			friend class Renderer;
			friend class Factory;
	};

}

