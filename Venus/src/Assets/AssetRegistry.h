#pragma once

#include <filesystem>
#include <unordered_map>
#include "Asset.h"

namespace Venus {

	class AssetRegistry
	{
		public:
			std::filesystem::path GetPathKey(const std::filesystem::path& path) const;

			AssetMetadata& operator[](const std::filesystem::path& path);
			const AssetMetadata& Get(const std::filesystem::path& path) const;

			size_t Count() const { return m_AssetRegistry.size(); }
			bool Contains(const std::filesystem::path& path) const;
			size_t Remove(const std::filesystem::path& path);
			void Clear();

			std::unordered_map<std::filesystem::path, AssetMetadata>::iterator begin() { return m_AssetRegistry.begin(); }
			std::unordered_map<std::filesystem::path, AssetMetadata>::iterator end() { return m_AssetRegistry.end(); }
			std::unordered_map<std::filesystem::path, AssetMetadata>::const_iterator cbegin() const { return m_AssetRegistry.cbegin(); }
			std::unordered_map<std::filesystem::path, AssetMetadata>::const_iterator cend() const { return m_AssetRegistry.cend(); }

		private:
			std::unordered_map<std::filesystem::path, AssetMetadata> m_AssetRegistry;

	};

}

