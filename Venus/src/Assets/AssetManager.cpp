#include "pch.h"
#include "AssetManager.h"

#include "AssetExtensions.h"
#include "ImGui/UI.h"

#include "yaml-cpp/yaml.h"

namespace Venus {

	std::unordered_map<AssetHandle, Ref<Asset>> AssetManager::s_LoadedAssets;
	std::unordered_map<AssetHandle, Ref<Asset>> AssetManager::s_MemoryAssets;

	void AssetManager::Init()
	{
		AssetImporter::Init();

		DeserializeRegistry();
		ReloadAssets();
	}

	void AssetManager::Shutdown()
	{
		SerializeRegistry();
		
		s_AssetRegistry.Clear();
		s_LoadedAssets.clear();
		s_MemoryAssets.clear();
	}

	const AssetHandle AssetManager::GetHandle(const std::filesystem::path& path)
	{
		return s_AssetRegistry.Contains(path) ? s_AssetRegistry[path].Handle : 0;
	}

	const AssetMetadata& AssetManager::GetMetadata(AssetHandle handle)
	{
		for (auto& [filepath, metadata] : s_AssetRegistry)
		{
			if (metadata.Handle == handle)
				return metadata;
		}

		return AssetMetadata();
	}

	const AssetMetadata& AssetManager::GetMetadata(const std::filesystem::path& path)
	{
		if (s_AssetRegistry.Contains(path))
			return s_AssetRegistry[path];

		return AssetMetadata();
	}


	AssetMetadata& AssetManager::GetMetadata_Internal(AssetHandle handle)
	{
		for (auto& [filepath, metadata] : s_AssetRegistry)
		{
			if (metadata.Handle == handle)
				return metadata;
		}

		return AssetMetadata();
	}

	AssetHandle AssetManager::ImportAsset(const std::filesystem::path& path)
	{
		std::filesystem::path relativePath = GetRelativePath(path);

		if (s_AssetRegistry.Contains(relativePath))
			return s_AssetRegistry[relativePath].Handle;

		std::string extension = path.extension().string();
		AssetType type = GetAssetType(extension);
		if (type == AssetType::None)
			return 0;

		AssetMetadata metadata;
		metadata.Handle = UUID();
		metadata.FilePath = relativePath;
		metadata.Type = type;

		s_AssetRegistry[relativePath] = metadata;

		return metadata.Handle;
	}

	std::filesystem::path AssetManager::GetPath(AssetHandle handle)
	{
		AssetMetadata metadata = GetMetadata(handle);
		return g_AssetsPath / metadata.FilePath;
	}

	std::filesystem::path AssetManager::GetRelativePath(const std::filesystem::path& path)
	{
		std::string temp = path.string();
		if (temp.find(g_AssetsPath.string()) != std::string::npos)
			return std::filesystem::relative(path, g_AssetsPath);

		return path;
	}

	AssetType AssetManager::GetAssetType(const std::string& extension)
	{
		if (s_AssetExtensionMap.find(extension) == s_AssetExtensionMap.end())
			return AssetType::None;

		return s_AssetExtensionMap.at(extension);
	}

	void AssetManager::ReloadAssets()
	{
		ProcessDirectory(g_AssetsPath); // root
		SerializeRegistry();
	}

	void AssetManager::SerializeRegistry()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "Assets" << YAML::BeginSeq;
		for (auto& [path, metadata] : s_AssetRegistry)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Handle" << YAML::Value << metadata.Handle;
			out << YAML::Key << "FilePath" << YAML::Value << metadata.FilePath.string();
			out << YAML::Key << "Type" << YAML::Value << Utils::AssetTypeToString(metadata.Type);
			out << YAML::EndMap;
		}
		YAML::EndSeq;
		YAML::EndMap;

		std::filesystem::path regPath = g_AssetsPath / "assets.vsreg";
		std::ofstream fout(regPath.string());
		fout << out.c_str();
		fout.close();
	}

	void AssetManager::DeserializeRegistry()
	{
		CORE_LOG_INFO("Loading Asset Registry...");

		std::filesystem::path regPath = g_AssetsPath / "assets.vsreg";

		if (!std::filesystem::exists(regPath))
		{
			CORE_LOG_WARN("Registry not found!");
			return;
		}

		std::ifstream stream(regPath);
		VS_CORE_ASSERT(stream);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		auto handles = data["Assets"];
		if (!handles)
		{
			CORE_LOG_ERROR("Failed to load Registry (maybe is corrupted)");
			return;
		}

		for (auto entry : handles)
		{
			AssetMetadata metadata;
			metadata.Handle = entry["Handle"].as<uint64_t>();
			metadata.FilePath = entry["FilePath"].as<std::string>();
			metadata.Type = Utils::AssetTypeFromString(entry["Type"].as<std::string>());

			if (metadata.Handle == 0 || metadata.Type == AssetType::None)
			{
				CORE_LOG_WARN("Handle or Type for Asset {0} is invalid!", metadata.FilePath.string());
				continue;
			}

			if (!std::filesystem::exists(g_AssetsPath / metadata.FilePath))
			{
				CORE_LOG_WARN("Missing Registry Asset '{0}', trying to locate...", metadata.FilePath.string());
				std::string mostLikelyCandidate;
				uint32_t bestScore = 0;

				for (auto& entry : std::filesystem::recursive_directory_iterator(g_AssetsPath))
				{
					const auto& path = entry.path();

					if (path.filename() != metadata.FilePath.filename())
						continue;

					if (bestScore > 0)
						CORE_LOG_TRACE("Multiple candidades found...");

					std::vector<std::string> candidateParts;
					{
						size_t start = 0;
						size_t end = path.string().find_first_of("/\\");
						while (end <= std::string::npos)
						{
							std::string token = path.string().substr(start, end - start);
							if (!token.empty())
								candidateParts.push_back(token);

							if (end == std::string::npos)
								break;

							start = end + 1;
							end = path.string().find_first_of("/\\", start);
						}
					}

					uint32_t score = 0;
					for (const auto& string : candidateParts)
					{
						if (metadata.FilePath.string().find(string) != std::string::npos)
							score++;
					}

					if (bestScore > 0 && score == bestScore)
					{
						// TODO Prompt User to decide...
					}

					if (score <= bestScore)
						continue;

					bestScore = score;
					mostLikelyCandidate = path.string();
				}

				if (mostLikelyCandidate.empty() && bestScore == 0)
				{
					CORE_LOG_ERROR("Failed to locate potential candidate to '{0}'", metadata.FilePath.string());
					continue;
				}

				std::replace(mostLikelyCandidate.begin(), mostLikelyCandidate.end(), '\\', '/');
				metadata.FilePath = std::filesystem::relative(mostLikelyCandidate, g_AssetsPath);
				CORE_LOG_TRACE("Found most likely candidate '{0}'", metadata.FilePath.string());
			}

			s_AssetRegistry[metadata.FilePath] = metadata;
		}

		CORE_LOG_INFO("Loaded {0} Asset entries from Registry", s_AssetRegistry.Count());
	}

	void AssetManager::OnAssetMoved(AssetHandle asset, const std::filesystem::path& newFilePath)
	{
		AssetMetadata metadata = GetMetadata(asset);
		if (!metadata.IsValid())
			return;

		s_AssetRegistry.Remove(metadata.FilePath);
		metadata.FilePath = newFilePath;
		s_AssetRegistry[metadata.FilePath] = metadata;

		SerializeRegistry();
	}

	void AssetManager::OnAssetRenamed(AssetHandle asset, const std::filesystem::path& newFilePath)
	{
		AssetMetadata metadata = GetMetadata(asset);
		if (!metadata.IsValid())
			return;

		s_AssetRegistry.Remove(metadata.FilePath);
		metadata.FilePath = s_AssetRegistry.GetPathKey(newFilePath);
		s_AssetRegistry[metadata.FilePath] = metadata;

		SerializeRegistry();
	}

	void AssetManager::OnAssetDeleted(AssetHandle asset)
	{
		AssetMetadata metadata = GetMetadata(asset);
		if (!metadata.IsValid())
			return;

		s_AssetRegistry.Remove(metadata.FilePath);
		// TODO: Delete asset from memory

		SerializeRegistry();
	}

	void AssetManager::ProcessDirectory(const std::filesystem::path& path)
	{
		for (auto entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
				ProcessDirectory(entry.path());
			else
				ImportAsset(entry.path());
		}
	}

	static int s_AssetsInspectorFilter = 0;
	static bool s_AssetsLoadedFilter = false;
	void AssetManager::OnImGuiRender(bool& show)
	{
		if (!show)
			return;

		ImGui::Begin(ICON_FA_STAR_O " Assets Inspector", &show);

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_FramePadding;

		// Filter
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 0.5f });
		UI::Text(ICON_FA_FILTER " Filter");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		UI::ShiftPosX(5.0f);
		ImGui::SetNextItemWidth(140.0f);
		const char* filter[] = { "None", "Texture", "Font", "Scene", "Model", "Script", "Environment Map", "Material" };
		UI::DropDown("ConsoleFilter", filter, 8, &s_AssetsInspectorFilter, false, false, false);
		
		ImGui::SameLine();
		UI::ShiftPosX(10.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 0.5f });
		UI::Text("Loaded");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::Checkbox("##Loaded", &s_AssetsLoadedFilter);

		// Search Assets
		ImGui::SameLine();
		UI::SetPosX(ImGui::GetWindowContentRegionMax().x - 270.0f);
		ImGui::TextDisabled(ICON_FA_SEARCH);
		ImGui::SameLine();
		static ImGuiTextFilter assetRegistryFilter;
		assetRegistryFilter.Draw("##Search", 250.0f);
		ImGui::Separator();

		for (const auto& [path, metadata] : s_AssetRegistry)
		{
			std::string assetName = path.stem().string();
			std::string handle = fmt::format("{0}", metadata.Handle);
			std::string filepath = metadata.FilePath.string();
			std::string type = Utils::AssetTypeToString(metadata.Type);

			if ((s_AssetsInspectorFilter == 0 || (AssetType)s_AssetsInspectorFilter == metadata.Type) &&
				assetRegistryFilter.PassFilter(assetName.c_str()))
			{
				if (s_AssetsLoadedFilter && !metadata.IsDataLoaded)
					continue;

				if (ImGui::TreeNodeEx(assetName.c_str(), treeNodeFlags))
				{
					UI::TextWithLabel("Handle", handle.c_str(), true);
					UI::TextWithLabel("Type", type.c_str(), true);
					UI::TextWithLabel("Path", filepath.c_str());
					ImGui::TreePop();
				}
			}
		}

		ImGui::End();
	}
}