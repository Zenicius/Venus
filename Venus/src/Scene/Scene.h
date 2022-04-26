#pragma once

#include "Engine/UUID.h"
#include "Engine/Timestep.h"
#include "Renderer/EditorCamera.h"
#include "SceneCamera.h"

#include "entt.hpp"

class b2World;

namespace Venus {
	
	class Entity;

	class Scene 
	{
		public:
			Scene();
			~Scene();

			static Ref<Scene> Copy(Ref<Scene> other);

			void OnRuntimeStart();
			void OnRuntimeStop();

			Entity CreateEntity(const std::string& name = std::string());
			Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
			Entity DuplicateEntity(Entity entity);
			void DestroyEntity(Entity entity);

			void OnUpdateEditor(Timestep ts, EditorCamera& camera);
			void OnUpdateRuntime(Timestep ts);
			void OnViewportResize(uint32_t width, uint32_t height);

			Entity GetEntityByUUID(UUID uuid);
			Entity GetPrimaryCamera();
			
			template<typename... Components>
			auto GetAllEntitiesWith()
			{
				return m_Registry.view<Components...>();
			}

		private:
			// TODO Better way to handle this
			template<typename T>
			void OnComponentAdded(Entity, T& component);

		private:
			std::string m_SceneName = "Untitled Scene";

			// Entities
			entt::registry m_Registry;

			// Physics 
			b2World* m_PhysicsWorld = nullptr;
			uint32_t m_VelocityIterations = 6;
			uint32_t m_PositionIterations = 2;

			uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

			friend class SceneSerializer;
			friend class Entity;
			friend class EditorLayer;
			friend class ObjectsPanel;
	};
}