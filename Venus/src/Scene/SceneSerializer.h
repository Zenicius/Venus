#pragma once

#include "Scene.h"

namespace Venus {

	class SceneSerializer
	{
		public:
			SceneSerializer(const Ref<Scene>& scene);

			void Serialize(const std::string& filepath);
			void SerializePrefab(UUID id, const std::string& filepath);
			void SerializeRuntime(const std::string& filepath);

			bool Deserialize(const std::string& filepath);
			Entity DeserializePrefab(const std::string& filepath);
			bool DeserializeRuntime(const std::string& filepath);

		private:
			Ref<Scene> m_Scene;
	};

}

