#pragma once

#include "entt.hpp"
#include "Engine/Timestep.h"

#include "SceneCamera.h"

namespace Venus {
	
	class Entity;

	class Scene 
	{
		public:
			Scene();
			~Scene();

			Entity CreateEntity(const std::string& name = std::string());
			void DestroyEntity(Entity entity);

			void OnUpdate(Timestep ts);
			void OnViewportResize(uint32_t width, uint32_t height);
		private:
			// TODO Better way to handle this
			template<typename T>
			void OnComponentAdded(Entity, T& component);

		private:
			entt::registry m_Registry;

			uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

			friend class Entity;
			friend class ObjectsPanel;
	};
}