#include "pch.h"
#include "Mesh.h"

#include "Engine/Timer.h"

#include "Assets/AssetManager.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/material.h>

namespace Venus {

	/////////////////////////////////////////////////////////////////////////////
	// Mesh /////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, uint32_t materialIndex)
		: m_Vertices(vertices), m_Indices(indices), m_MaterialIndex(materialIndex)
	{
		InitMesh();
	}

	void Mesh::InitMesh()
	{
		m_VertexArray = VertexArray::Create();
		m_VertexBuffer = VertexBuffer::Create(&m_Vertices[0], m_Vertices.size() * sizeof(Vertex));

		m_IndexBuffer = IndexBuffer::Create(&m_Indices[0], m_Indices.size());

		m_VertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal"   },
			{ ShaderDataType::Float3, "a_Tangent"  },
			{ ShaderDataType::Float3, "a_Binormal" },
			{ ShaderDataType::Float2, "a_TexCoord" },
		});

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Model ////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	Ref<Model> Model::Create(const std::string& path)
	{
		return CreateRef<Model>(path);
	}

	Model::Model()
	{
	}

	Model::Model(const std::string& path)
		: m_Path(path)
	{
		LoadModel();
	}

	void Model::LoadModel()
	{
		Timer timer;

		Assimp::Importer importer;

		const uint32_t meshImportFlags =
			aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
			aiProcess_Triangulate |             // Make sure we're triangles
			aiProcess_SortByPType |             // Split meshes by primitive type
			aiProcess_GenNormals |              // Make sure we have legit normals
			aiProcess_GenUVCoords |             // Convert UVs if required 
	//		aiProcess_OptimizeGraph |
			aiProcess_OptimizeMeshes |          // Batch draws where possible
			aiProcess_JoinIdenticalVertices |
			aiProcess_GlobalScale |             // e.g. convert cm to m for fbx import (and other formats where cm is native)
			aiProcess_ValidateDataStructure;    // Validation


		const uint32_t oldImportFlags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;

		const aiScene* scene = importer.ReadFile(m_Path, oldImportFlags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			CORE_LOG_ERROR("Failed to load model! : {0}", importer.GetErrorString());
		else
		{
			uint32_t materialCount = scene->mNumMaterials;
			m_Materials = CreateRef<MaterialTable>(materialCount);

			CORE_LOG_TRACE("Loading Model: {0}", m_Path);
			ProcessNode(scene->mRootNode, scene);
		}

		CORE_LOG_WARN("Model {0} loading took: {1} ms", m_Path, timer.ElapsedMillis());
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_Meshes.push_back(ProcessMesh(mesh, scene));
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		// Vertices
		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector;

			// Positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			// Normals
			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
			}

			// Textures coordinates
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
				// tangent
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangent = vector;
				// bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.Bitangent = vector;
			}
			else
			{
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}

			vertices.push_back(vertex);
		}

		// Indices
		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		// Material
		uint32_t materialIndex = mesh->mMaterialIndex;
		aiMaterial* material = scene->mMaterials[materialIndex];

		if (!m_Materials->HasMaterial(materialIndex))
		{
			aiString materialName = material->GetName();
			CORE_LOG_TRACE("Loading Default Material '{0}' at: {1}", materialName.C_Str(), materialIndex);

			Ref<MeshMaterial> meshMaterial = MeshMaterial::Create(materialName.C_Str());

			aiColor3D aiColor, aiEmission;
			// Albedo Color
			if (material->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) == AI_SUCCESS)
			{
				CORE_LOG_TRACE("	Setting Albedo Color : {0},{1},{2} ", aiColor.r, aiColor.g, aiColor.b);
				meshMaterial->SetAlbedoColor( { aiColor.r, aiColor.g, aiColor.b } );
			}
			// Emission
			if (material->Get(AI_MATKEY_COLOR_EMISSIVE, aiEmission) == AI_SUCCESS)
			{
				CORE_LOG_TRACE("	Setting Emission : {0}", aiEmission.r);
				meshMaterial->SetEmission(aiEmission.r);
			}
			float metalness, shininess;
			// Metalness
			if (material->Get(AI_MATKEY_REFLECTIVITY, metalness) == AI_SUCCESS)
			{
				CORE_LOG_TRACE("	Setting Metalness : {0}", metalness);
				meshMaterial->SetMetalness(metalness);
			}
			// Roughness
			if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
			{
				float roughness = 1.0f - glm::sqrt(shininess / 100.0f);
				if (roughness < 0)
					roughness = 0;
				CORE_LOG_TRACE("	Setting Roughness : {0}", roughness);
				meshMaterial->SetRoughtness(roughness);
			}

			aiString aiTexPath;
			// Albedo Map
			bool hasAlbedoMap = material->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS;
			if (hasAlbedoMap)
			{
				CORE_LOG_TRACE("	Loading Albedo Map : {0}", aiTexPath.C_Str());

				std::filesystem::path modelPath = std::filesystem::path(m_Path).parent_path();
				std::string finalTexPath = modelPath.string() + "/" + aiTexPath.C_Str();

				Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(finalTexPath);

				if (texture && texture->IsLoaded())
				{
					TextureProperties props;
					props.Format = TextureFormat::SRGB;
					texture->SetProperties(props, true);
					meshMaterial->SetAlbedoMap(texture);
				}
			}
			// Normal Map 
			bool hasNormalMap = material->GetTexture(aiTextureType_NORMALS, 0, &aiTexPath) == AI_SUCCESS ||
								material->GetTexture(aiTextureType_DISPLACEMENT, 0, &aiTexPath) == AI_SUCCESS ||
								material->GetTexture(aiTextureType_HEIGHT, 0, &aiTexPath) == AI_SUCCESS;
			if (hasNormalMap)
			{
				CORE_LOG_TRACE("	Loading Normal Map : {0}", aiTexPath.C_Str());

				std::filesystem::path modelPath = std::filesystem::path(m_Path).parent_path();
				std::string finalTexPath = modelPath.string() + "/" + aiTexPath.C_Str();

				Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(finalTexPath);
				if (texture && texture->IsLoaded())
					meshMaterial->SetNormalMap(texture);
			}
			// Roughness Map
			bool hasRoughnessMap = material->GetTexture(aiTextureType_SHININESS, 0, &aiTexPath) == AI_SUCCESS;
			if (hasRoughnessMap)
			{
				CORE_LOG_TRACE("	Loading Roughness Map : {0}", aiTexPath.C_Str());

				std::filesystem::path modelPath = std::filesystem::path(m_Path).parent_path();
				std::string finalTexPath = modelPath.string() + "/" + aiTexPath.C_Str();

				Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(finalTexPath);
				if (texture && texture->IsLoaded())
					meshMaterial->SetRoughnessMap(texture);
			}
			
			//TODO: Metalness

			m_Materials->SetMaterial(materialIndex, meshMaterial);
		}
		else
		{
			//CORE_LOG_TRACE("Default Material already loaded {0}", materialIndex);
		}
			
		return Mesh(vertices, indices, materialIndex);
	}

	void Model::LogMeshStatistics(const aiScene* scene)
	{
		CORE_LOG_TRACE("Total Meshs: {0}", m_Meshes.size());

		uint32_t totalVertices = 0;
		uint32_t totalIndices = 0;
		for (const auto& mesh : m_Meshes)
		{
			totalVertices += mesh.m_Vertices.size();
			totalIndices += mesh.m_Indices.size();
		}

		CORE_LOG_TRACE("Total Vertices: {0}", totalVertices);
		CORE_LOG_TRACE("Total Indices: {0}", totalIndices);

		CORE_LOG_TRACE("Total Materials: {0}", scene->mNumMaterials);

		for (uint32_t i = 0; i < scene->mNumMaterials; i++)
		{
			CORE_LOG_TRACE("	Total Diffuse: {0}", scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE));
			CORE_LOG_TRACE("	Total Normal: {0}", scene->mMaterials[i]->GetTextureCount(aiTextureType_NORMALS));
			CORE_LOG_TRACE("	Total Roughness: {0}", scene->mMaterials[i]->GetTextureCount(aiTextureType_SHININESS));
		}
	}

}