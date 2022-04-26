#pragma once

namespace Venus {

	class AssetSerializer
	{
		public:
			AssetSerializer();
			
			void Serialize(const std::string& assetPath, int filterMode, int wrapMode);
			std::tuple<int, int> Deserialize(const std::string& assetPath);
	};

}
