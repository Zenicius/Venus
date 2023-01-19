#pragma once

#include "Assets/Asset.h"
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

	class Scene : public Asset
	{
		public:
			Scene();
			~Scene();

			static Ref<Scene> Copy(Ref<Scene> other);

			void OnRuntimeStart();
			void OnSimulationStart();
			void OnRuntimeStop();

			void OnUpdateEditor(Ref<SceneRenderer> renderer, Timestep ts, EditorCamera& camera);
			void OnUpdateRuntime(Ref<SceneRenderer> renderer, Timestep ts);
			void OnUpdateSimulation(Ref<SceneRenderer> renderer, Timestep ts, EditorCamera& camera);
			void OnViewportResize(uint32_t width, uint32_t height);

			void PauseScene(bool value) { m_IsPaused = value; }
			bool IsPaused() { return m_IsPaused; }

			//--- Entity Managament--------------------------------------------------------------
			Entity CreateEntity(const std::string& name = std::string());
			Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
			Entity CreateChildEntity(Entity parent, const std::string& name = std::string());

			void ParentEntity(Entity child, Entity parent);
			void UnparentEntity(Entity child, bool toWorldSpace = true);
			
			Entity GetEntityWithUUID(UUID id) const;
			Entity TryGetEntityWithUUID(UUID id) const;
			Entity TryGetEntityWithName(const std::string& name);

			template<typename... Components>
			auto GetAllEntitiesWith()
			{
				return m_Registry.view<Components...>();
			}

			Entity DuplicateEntity(Entity entity);
			void DestroyEntity(Entity entity, bool destroyChildren = true, bool first = true);
			void SubmitToDestroyEntity(Entity entity);
			//-----------------------------------------------------------------------------------

			void SetEditorSelectedEntity(uint32_t entity) { m_EditorSelectedEntity = entity; }
			Entity GetPrimaryCamera();

			glm::vec3 GetWorldSpacePosition(Entity entity);
			TransformComponent GetWorldSpaceTransform(Entity entity);
			glm::mat4 GetWorldSpaceTransformMatrix(Entity entity);
			void ConvertToLocalSpace(Entity entity);
			void ConvertToWorldSpace(Entity entity);

			//---
			static AssetType GetStaticType() { return AssetType::Scene; }
			virtual AssetType GetAssetType() const override { return GetStaticType(); }

		private:
			// TODO Better way to handle this
			template<typename T>
			void OnComponentAdded(Entity, T& component);

			std::vector<std::function<void()>> m_PostUpdateQueue;
			template<typename Fn>
			void SubmitPostUpdateFn(Fn&& fn)
			{
				m_PostUpdateQueue.emplace_back(fn);
			}

		private:
			std::string m_SceneName = "Untitled Scene";
			uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
			bool m_IsPaused = false;

			// Entities
			entt::registry m_Registry;
			uint32_t m_EditorSelectedEntity = -1;
			std::unordered_map<UUID, Entity> m_EntityMap;

			LightEnvironment m_LightEnvironment;

			// Scripting
			bool m_ReloadAssembliesOnPlay = true;

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