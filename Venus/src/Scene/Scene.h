#pragma once

#include "Engine/Timestep.h"
#include "Renderer/EditorCamera.h"
#include "SceneCamera.h"

#include "entt.hpp"

namespace Venus {
	
	class Entity;

	class Scene 
	{
		public:
			Scene();
			~Scene();

			Entity CreateEntity(const std::string& name = std::string());
			void DestroyEntity(Entity entity);

			void OnUpdateEditor(Timestep ts, EditorCamera& camera);
			void OnUpdateRuntime(Timestep ts);
			void OnViewportResize(uint32_t width, uint32_t height);

			Entity GetPrimaryCamera();

		private:
			// TODO Better way to handle this
			template<typename T>
			void OnComponentAdded(Entity, T& component);

		private:
			entt::registry m_Registry;

			std::string m_SceneName = "Untitled Scene";

			uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

			friend class SceneSerializer;
			friend class Entity;
			friend class EditorLayer;
			friend class ObjectsPanel;
	};
}