#include "pch.h"
#include "Mesh.h"

#include "Engine/Timer.h"

#include "glad/glad.h"

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

	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<MaterialTexture> textures)
		: m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
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

	Model::Model()
	{
	}

	Model::Model(const std::string& path)
		: m_Path(path)
	{
		LoadModel();
		Init();
	}

	void Model::LoadModel()
	{
		Timer timer;

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(m_Path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			CORE_LOG_ERROR("Failed to load model! : {0}", importer.GetErrorString());
		else
		{
			CORE_LOG_TRACE("Loading Model: {0}", m_Path);
			ProcessNode(scene->mRootNode, scene);
		}

		CORE_LOG_WARN("Model {0} loading took: {1} ms", m_Path, timer.ElapsedMillis());
	}

	void Model::Init()
	{
		/*
		for (auto& mesh : m_Meshes)
		{
			m_ModelVertices.insert(m_ModelVertices.end(), mesh.m_Vertices.begin(), mesh.m_Vertices.end());
			m_ModelIndices.insert(m_ModelIndices.end(), mesh.m_Indices.begin(), mesh.m_Indices.end());
		}

		CORE_LOG_TRACE("Model Vertices: {0}", m_ModelVertices.size());
		CORE_LOG_TRACE("Model Indices: {0}", m_ModelIndices.size());
	
		m_VertexBuffer = VertexBuffer::Create(m_ModelVertices.size() * sizeof(Vertex));
		m_VertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal"   },
			{ ShaderDataType::Float3, "a_Tangent"  },
			{ ShaderDataType::Float3, "a_Binormal" },
			{ ShaderDataType::Float2, "a_TexCoord" },
		});
		m_VertexBuffer->SetData(&m_ModelVertices[0], m_ModelVertices.size() * sizeof(Vertex));

		m_IndexBuffer = IndexBuffer::Create(&m_ModelIndices[0], m_ModelIndices.size());
		*/
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
		std::vector<MaterialTexture> textures;

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

		// Materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];	
		
		std::vector<MaterialTexture> diffuseMaps = LoadMaterialTextures(material, TextureType::Diffuse);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<MaterialTexture> specularMaps = LoadMaterialTextures(material, TextureType::Specular);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<MaterialTexture> normalMaps = LoadMaterialTextures(material, TextureType::Normal);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		
		return Mesh(vertices, indices, textures);
	}

	void Model::LogMeshStatistics()
	{
		CORE_LOG_TRACE("Total Meshs: {0}", m_Meshes.size());

		uint32_t totalVertices = 0;
		uint32_t totalIndices = 0;
		uint32_t totalTextures = 0;
		for (const auto& mesh : m_Meshes)
		{
			totalVertices += mesh.m_Vertices.size();
			totalIndices += mesh.m_Indices.size();
			totalTextures += mesh.m_Textures.size();
		}

		CORE_LOG_TRACE("Total Vertices: {0}", totalVertices);
		CORE_LOG_TRACE("Total Indices: {0}", totalIndices);
		CORE_LOG_TRACE("Total Textures: {0}", totalTextures);
	}

	static aiTextureType VenusTextureTypeToAssimType(TextureType type)
	{
		switch (type)
		{
			case TextureType::Diffuse:	return aiTextureType::aiTextureType_DIFFUSE;
			case TextureType::Normal:	return aiTextureType::aiTextureType_HEIGHT;
			case TextureType::Specular:	return aiTextureType::aiTextureType_SPECULAR;
		}

		VS_CORE_ASSERT(false, "Unknown Material Texture Type");
		return aiTextureType::aiTextureType_NONE;
	}

	std::vector<MaterialTexture> Model::LoadMaterialTextures(aiMaterial* material, TextureType type)
	{
		std::vector<MaterialTexture> textures;

		aiTextureType assimpType = VenusTextureTypeToAssimType(type);
		if (assimpType == aiTextureType::aiTextureType_NONE)
			return textures;
		
		for (uint32_t i = 0; i < material->GetTextureCount(assimpType); i++)
		{
			MaterialTexture materialTexture;
			materialTexture.Type = type;
			
			aiString texName;
			material->GetTexture(assimpType, i, &texName);

			std::filesystem::path modelPath = std::filesystem::path(m_Path).parent_path();
			std::string finalTexPath = modelPath.string() + "/" + texName.C_Str();

			switch (type)
			{
				case TextureType::Diffuse:
				{
					bool isLoaded = false;
					for (uint32_t i = 0; i < m_DiffuseMaps.size(); i++)
					{
						if (std::strcmp(finalTexPath.c_str(), m_DiffuseMaps[i]->GetPath().c_str()) == 0)
						{
							isLoaded = true;
							materialTexture.Index = i;
							break;
						}
					}

					if (!isLoaded)
					{
						CORE_LOG_TRACE("Loading Diffuse Map Texture: {0}", finalTexPath);
						materialTexture.Index = m_DiffuseMaps.size();
						m_DiffuseMaps.push_back(Texture2D::Create(finalTexPath));
					}

					textures.push_back(materialTexture);

					break;
				}

				case TextureType::Specular:
				{
					bool isLoaded = false;
					for (uint32_t i = 0; i < m_SpecularMaps.size(); i++)
					{
						if (std::strcmp(finalTexPath.c_str(), m_SpecularMaps[i]->GetPath().c_str()) == 0)
						{
							isLoaded = true;
							materialTexture.Index = i;
							break;
						}
					}

					if (!isLoaded)
					{
						CORE_LOG_TRACE("Loading Specular Map Texture: {0}", finalTexPath);
						materialTexture.Index = m_SpecularMaps.size();
						m_SpecularMaps.push_back(Texture2D::Create(finalTexPath));
					}

					textures.push_back(materialTexture);

					break;
				}

				case TextureType::Normal:
				{
					bool isLoaded = false;
					for (uint32_t i = 0; i < m_NormalMaps.size(); i++)
					{
						if (std::strcmp(finalTexPath.c_str(), m_NormalMaps[i]->GetPath().c_str()) == 0)
						{
							isLoaded = true;
							materialTexture.Index = i;
							break;
						}
					}

					if (!isLoaded)
					{
						materialTexture.Index = m_NormalMaps.size();
						CORE_LOG_TRACE("Loading Normal Map Texture: {0}", finalTexPath);
						m_NormalMaps.push_back(Texture2D::Create(finalTexPath));
					}

					textures.push_back(materialTexture);

					break;
				}
			}
		}

		return textures;
	}
}