#pragma once

#include "Scene/Components.h"
#include "Scene/SceneEnvironment.h"
#include "Engine/UUID.h"
#include "Engine/Timestep.h"
#include "Renderer/EditorCamera.h"
#include "SceneCamera.h"

#include "entt.hpp"

class b2World;

namespace Venus {
	
	class SceneRenderer;
	class Entity;

	struct DirectionalLight
	{
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Color = { 0.0f, 0.0f, 0.0f };
		float Intensity = 0.0f;
		bool CastShadows = true;
	};

	struct PointLight
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float Intensity = 0.0f;
		glm::vec3 Color = { 0.0f, 0.0f, 0.0f };
		float MinRadius = 0.001f;
		float Radius = 25.0f;
		float Falloff = 1.f;
		float SourceSize = 0.1f;
		bool CastsShadows = true;
		char Padding[3]{ 0, 0, 0 };
	};

	struct SkyLight
	{
		Ref<SceneEnvironment> EnvironmentMap = nullptr;
		float Intensity = 1.0f;
		float Lod = 0.0f;
	};

	struct LightEnvironment
	{
		DirectionalLight DirectionalLight;
		std::vector<PointLight> PointLights;
		SkyLight SkyLight;
		bool HasDirLight = false;
		bool CastsShadows = true;
	};

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

			void OnUpdateEditor(Ref<SceneRenderer> renderer, Timestep ts, EditorCamera& camera);
			void OnUpdateRuntime(Ref<SceneRenderer> renderer, Timestep ts);
			void OnOverlayRender(EditorCamera& camera);
			void OnViewportResize(uint32_t width, uint32_t height);

			Entity GetEntityByUUID(UUID uuid);
			Entity GetPrimaryCamera();

			void SetEditorSelectedEntity(uint32_t entity) { m_EditorSelectedEntity = entity; }
			
			glm::vec3 GetWorldSpacePosition(Entity entity);

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
			uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

			// Entities
			entt::registry m_Registry;
			uint32_t m_EditorSelectedEntity = -1;

			LightEnvironment m_LightEnvironment;

			// Physics 2D
			b2World* m_PhysicsWorld = nullptr;
			uint32_t m_VelocityIterations = 6;
			uint32_t m_PositionIterations = 2;

			friend class SceneRenderer;
			friend class SceneSerializer;
			friend class Entity;
			friend class EditorLayer;
			friend class ObjectsPanel;
	};
}