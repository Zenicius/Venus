#include "pch.h"
#include "AssetRegistry.h"

namespace Venus {

	extern const std::filesystem::path g_AssetsPath;

	std::filesystem::path AssetRegistry::GetPathKey(const std::filesystem::path& path) const
	{
		auto key = std::filesystem::relative(path, g_AssetsPath);
		if (key.empty())
			key = path.lexically_normal();

		return key;
	}

	AssetMetadata& AssetRegistry::operator[](const std::filesystem::path& path)
	{
		VS_CORE_ASSERT(!path.string().empty());
		auto key = GetPathKey(path);
		return m_AssetRegistry[key];
	}

	const AssetMetadata& AssetRegistry::Get(const std::filesystem::path& path) const
	{
		VS_CORE_ASSERT(!path.string().empty());
		auto key = GetPathKey(path);
		return m_AssetRegistry.at(key);
	}

	bool AssetRegistry::Contains(const std::filesystem::path& path) const
	{
		auto key = GetPathKey(path);
		return m_AssetRegistry.find(key) != m_AssetRegistry.end();
	}

	size_t AssetRegistry::Remove(const std::filesystem::path& path)
	{
		auto key = GetPathKey(path);
		return m_AssetRegistry.erase(key);
	}

	void AssetRegistry::Clear()
	{
		m_AssetRegistry.clear();
	}

}