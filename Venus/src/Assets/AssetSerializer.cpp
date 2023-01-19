#include "pch.h"
#include "AssetSerializer.h"

#include "AssetManager.h"
#include "Renderer/Renderer.h"
#include "Scene/SceneSerializer.h"
#include "Utils/SerializationUtils.h"

namespace Venus {

	/////////////////////////////////////////////////////////////////////////////
	// MODEL ////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	bool ModelSerializer::TryLoadAsset(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		asset = Model::Create(AssetManager::GetPath(metadata.Handle).string());
		asset->Handle = metadata.Handle;

		bool result = std::dynamic_pointer_cast<Model>(asset)->GetMeshs().size() > 0;

		return result;
	}

	/////////////////////////////////////////////////////////////////////////////
	// TEXTURE //////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	bool TextureSerializer::TryLoadAsset(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		asset = Texture2D::Create(AssetManager::GetPath(metadata.Handle).string());
		asset->Handle = metadata.Handle;

		bool result = std::dynamic_pointer_cast<Texture2D>(asset)->IsLoaded();

		if (!result)
			asset->SetFlag(AssetFlag::Invalid);

		return result;
	}

	/////////////////////////////////////////////////////////////////////////////
	// MATERIAL /////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	void MaterialSerializer::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		Ref<MeshMaterial> material = std::dynamic_pointer_cast<MeshMaterial>(asset);

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Material" << YAML::Value;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "AlbedoColor" << YAML::Value << material->GetAlbedoColor();
			out << YAML::Key << "Emission" << YAML::Value << material->GetEmission();
			out << YAML::Key << "UseNormalMap" << YAML::Value << material->IsUsingNormalMap();
			out << YAML::Key << "Metalness" << YAML::Value << material->GetMetalness();
			out << YAML::Key << "Roughness" << YAML::Value << material->GetRoughness();

			Ref<Texture2D> defaultTexture = Renderer::GetDefaultTexture();
			if (defaultTexture != material->GetAlbedoMap())
			{
				AssetHandle albedoHandle = material->GetAlbedoMap()->Handle;
				out << YAML::Key << "AlbedoMap" << YAML::Value << albedoHandle;
			}
			if (defaultTexture != material->GetNormalMap())
			{
				AssetHandle normalHandle = material->GetNormalMap()->Handle;
				out << YAML::Key << "NormalMap" << YAML::Value << normalHandle;
			}
			if (defaultTexture != material->GetMetalnessMap())
			{
				AssetHandle metalnessHandle = material->GetMetalnessMap()->Handle;
				out << YAML::Key << "MetalnessMap" << YAML::Value << metalnessHandle;
			}
			if (defaultTexture != material->GetRoughnessMap())
			{
				AssetHandle roughnessHandle = material->GetRoughnessMap()->Handle;
				out << YAML::Key << "RoughnessMap" << YAML::Value << roughnessHandle;
			}

			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		std::ofstream fout(AssetManager::GetPath(metadata.Handle));
		fout << out.c_str();
		fout.close();
	}

	bool MaterialSerializer::TryLoadAsset(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(AssetManager::GetPath(metadata.Handle).string());
		}
		catch (YAML::ParserException e)
		{
			return false;
		}

		if (!data["Material"])
			return false;

		Ref<MeshMaterial> material = MeshMaterial::Create(AssetManager::GetPath(metadata.Handle).stem().string());

		auto materialData = data["Material"];

		if (materialData["AlbedoColor"])
			material->SetAlbedoColor(materialData["AlbedoColor"].as<glm::vec3>());
		if (materialData["Emission"])
			material->SetEmission(materialData["Emission"].as<float>());
		if (materialData["UseNormalMap"])
			material->SetUseNormalMap(materialData["UseNormalMap"].as<int>());
		if (materialData["Metalness"])
			material->SetMetalness(materialData["Metalness"].as<float>());
		if (materialData["Roughness"])
			material->SetRoughtness(materialData["Roughness"].as<float>());

		if (materialData["AlbedoMap"])
		{
			AssetHandle handle = materialData["AlbedoMap"].as<uint64_t>();
			if (AssetManager::IsAssetHandleValid(handle))
			{
				Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(handle);
				if (texture->GetProperties().Format != TextureFormat::SRGB)
				{
					TextureProperties props;
					props.Format = TextureFormat::SRGB;
					texture->SetProperties(props, true);
				}

				material->SetAlbedoMap(texture);
			}
		}
		if (materialData["NormalMap"])
		{
			AssetHandle handle = materialData["NormalMap"].as<uint64_t>();
			if (AssetManager::IsAssetHandleValid(handle))
				material->SetNormalMap(AssetManager::GetAsset<Texture2D>(handle));
		}
		if (materialData["MetalnessMap"])
		{
			AssetHandle handle = materialData["MetalnessMap"].as<uint64_t>();
			if (AssetManager::IsAssetHandleValid(handle))
				material->SetMetalnessMap(AssetManager::GetAsset<Texture2D>(handle));
		}
		if (materialData["RoughnessMap"])
		{
			AssetHandle handle = materialData["RoughnessMap"].as<uint64_t>();
			if (AssetManager::IsAssetHandleValid(handle))
				material->SetRoughnessMap(AssetManager::GetAsset<Texture2D>(handle));
		}

		asset = material;
		asset->Handle = metadata.Handle;

		return true;
	}

	/////////////////////////////////////////////////////////////////////////////
	// SCENE ASSET //////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	void SceneAssetSerializer::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const
	{
		Ref<Scene> scene = std::dynamic_pointer_cast<Scene>(asset);
		SceneSerializer serializer(scene);
		serializer.Serialize(AssetManager::GetPath(scene->Handle).string());
	}

	bool SceneAssetSerializer::TryLoadAsset(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		asset = CreateRef<Asset>();
		asset->Handle = metadata.Handle;
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////
	// ENVIRONMENT //////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	bool EnvironmentSerializer::TryLoadAsset(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		asset = Renderer::CreateEnvironmentMap(AssetManager::GetPath(metadata.Handle).string());
		asset->Handle = metadata.Handle;

		return true;
	}
}
