#include "pch.h"
#include "AssetSerializer.h"

#include "yaml-cpp/yaml.h"

namespace Venus {

	AssetSerializer::AssetSerializer()
	{
	}

	void AssetSerializer::Serialize(const std::string& assetPath, int filterMode, int wrapMode)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;
		out << YAML::Key << "Asset" << YAML::Value << assetPath;
		out << YAML::Key << "FilterMode" << YAML::Value << filterMode;
		out << YAML::Key << "WrapMode" << YAML::Value << wrapMode;
		out << YAML::EndMap;

		std::ofstream fout(assetPath + ".meta");
		fout << out.c_str();
		fout.close();
	}

	std::tuple<int, int> AssetSerializer::Deserialize(const std::string& assetPath)
	{
		YAML::Node data;
		try
		{
			std::filesystem::path path(assetPath + ".meta");
			if(std::filesystem::exists(path))
				data = YAML::LoadFile(assetPath + ".meta");
		}
		catch (YAML::ParserException e)
		{
			return std::make_tuple(-1, -1);
		}

		if (data)
		{
			int filterMode = data["FilterMode"].as<int>();
			int wrapMode = data["WrapMode"].as<int>();

			return std::make_tuple(filterMode, wrapMode);
		}
	}

}