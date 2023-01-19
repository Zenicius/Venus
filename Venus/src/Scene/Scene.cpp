#include "pch.h"
#include "Scene.h"

#include "Math/Math.h"

#include "Components.h"
#include "Entity.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Renderer.h"
#include "Renderer/SceneRenderer.h"

#include "Assets/AssetManager.h"
#include "Scripting/ScriptingEngine.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

namespace Venus {

	static b2BodyType Venus2DBodyTypeToBox2DType(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case Rigidbody2DComponent::BodyType::Static:    return b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		VS_CORE_ASSERT(false, "Unknown 2D Body type");
		return b2_staticBody;
	}

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	template<typename Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = src.view<Component>();
		for (auto e : view)
		{
			UUID uuid = src.get<IDComponent>(e).ID;
			VS_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());
			entt::entity enttID = enttMap.at(uuid);

			auto& component = src.get<Component>(e);
			dst.emplace_or_replace<Component>(enttID, component);
		}
	}

	template<typename Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		if (src.HasComponent<Component>())
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_SceneName = other->m_SceneName;

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		newScene->m_PositionIterations = other->m_PositionIterations;
		newScene->m_VelocityIterations = other->m_VelocityIterations;

		newScene->m_ReloadAssembliesOnPlay = other->m_ReloadAssembliesOnPlay;

		auto& srcRegistry = other->m_Registry;
		auto& destRegistry = newScene->m_Registry;

		std::unordered_map<UUID, entt::entity> enttMap;

		auto idView = srcRegistry.view<IDComponent>();
		// Create Entities
		for (auto e : idView)
		{
			UUID uuid = srcRegistry.get<IDComponent>(e).ID;
			const auto& name = srcRegistry.get<TagComponent>(e).Name;

			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);

			enttMap[uuid] = (entt::entity)newEntity;
		}

		// Copy components
		CopyComponent<TagComponent>(destRegistry, srcRegistry, enttMap);
		CopyComponent<TransformComponent>(destRegistry, srcRegistry, enttMap);
		CopyComponent<RelationshipComponent>(destRegistry, srcRegistry, enttMap);
		CopyComponent<SpriteRendererComponent>(destRegistry, srcRegistry, enttMap);
		CopyComponent<CircleRendererComponent>(destRegistry, srcRegistry, enttMap);
		CopyComponent<CameraComponent>(destRegistry, srcRegistry, enttMap);
		CopyComponent<Rigidbody2DComponent>(destRegistry, srcRegistry, enttMap);
		CopyComponent<BoxCollider2DComponent>(destRegistry, srcRegistry, enttMap);
		CopyComponent<CircleCollider2DComponent>(destRegistry, srcRegistry, enttMap);
		CopyComponent<MeshRendererComponent>(destRegistry, srcRegistry, enttMap);
		CopyComponent<PointLightComponent>(destRegistry, srcRegistry, enttMap);
		CopyComponent<DirectionalLightComponent>(destRegistry, srcRegistry, enttMap);
		CopyComponent<SkyLightComponent>(destRegistry, srcRegistry, enttMap);
		CopyComponent<ScriptComponent>(destRegistry, srcRegistry, enttMap);

		return newScene;
	}

	void Scene::OnRuntimeStart()
	{
		// Physics
		{
			m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };

				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb = entity.GetComponent<Rigidbody2DComponent>();

				//TODO: Fix Bug  child entities colliders in wrong positions
				b2BodyDef bodyDef;
				bodyDef.type = Venus2DBodyTypeToBox2DType(rb.Type);
				bodyDef.position.Set(transform.Position.x, transform.Position.y);
				bodyDef.angle = transform.Rotation.z;

				b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);;
				body->SetFixedRotation(rb.FixedRotation);

				rb.RuntimeBody = body;

				if (entity.HasComponent<BoxCollider2DComponent>())
				{
					auto& bc = entity.GetComponent<BoxCollider2DComponent>();

					b2PolygonShape shape;
					shape.SetAsBox(bc.Size.x * transform.Scale.x, bc.Size.y * transform.Scale.y,
						b2Vec2(bc.Offset.x, bc.Offset.y), transform.Rotation.z);

					b2FixtureDef fixture;
					fixture.shape = &shape;
					fixture.density = bc.Density;
					fixture.friction = bc.Friction;
					fixture.restitution = bc.Restitution;
					fixture.restitutionThreshold = bc.RestitutionThreshold;

					body->CreateFixture(&fixture);
				}

				else if (entity.HasComponent<CircleCollider2DComponent>())
				{
					auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

					b2CircleShape circleShape;
					circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
					circleShape.m_radius = transform.Scale.x * cc2d.Radius;

					b2FixtureDef fixture;
					fixture.shape = &circleShape;
					fixture.density = cc2d.Density;
					fixture.friction = cc2d.Friction;
					fixture.restitution = cc2d.Restitution;
					fixture.restitutionThreshold = cc2d.RestitutionThreshold;

					body->CreateFixture(&fixture);
				}
			}
		}

		// Scripting
		{
			if(m_ReloadAssembliesOnPlay)
				ScriptingEngine::Reload();

			ScriptingEngine::SetContext(this);
		
			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };

				auto& scriptComponent = entity.GetComponent<ScriptComponent>();

				if (ScriptingEngine::ModuleExists(scriptComponent.ModuleName))
				{
					ScriptingEngine::Instantiate(entity);
					ScriptingEngine::OnCreate(entity);
				}
			}
		}
	}

	void Scene::OnSimulationStart()
	{
		// Physics
		{
			m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };

				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb = entity.GetComponent<Rigidbody2DComponent>();

				b2BodyDef bodyDef;
				bodyDef.type = Venus2DBodyTypeToBox2DType(rb.Type);
				bodyDef.position.Set(transform.Position.x, transform.Position.y);
				bodyDef.angle = transform.Rotation.z;

				b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);;
				body->SetFixedRotation(rb.FixedRotation);

				rb.RuntimeBody = body;

				if (entity.HasComponent<BoxCollider2DComponent>())
				{
					auto& bc = entity.GetComponent<BoxCollider2DComponent>();

					b2PolygonShape shape;
					shape.SetAsBox(bc.Size.x * transform.Scale.x, bc.Size.y * transform.Scale.y,
						b2Vec2(bc.Offset.x, bc.Offset.y), transform.Rotation.z);

					b2FixtureDef fixture;
					fixture.shape = &shape;
					fixture.density = bc.Density;
					fixture.friction = bc.Friction;
					fixture.restitution = bc.Restitution;
					fixture.restitutionThreshold = bc.RestitutionThreshold;

					body->CreateFixture(&fixture);
				}

				else if (entity.HasComponent<CircleCollider2DComponent>())
				{
					auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

					b2CircleShape circleShape;
					circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
					circleShape.m_radius = transform.Scale.x * cc2d.Radius;

					b2FixtureDef fixture;
					fixture.shape = &circleShape;
					fixture.density = cc2d.Density;
					fixture.friction = cc2d.Friction;
					fixture.restitution = cc2d.Restitution;
					fixture.restitutionThreshold = cc2d.RestitutionThreshold;

					body->CreateFixture(&fixture);
				}
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		// Physics
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;

		// Scripting
		ScriptingEngine::ClearEntityData();
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };

		auto& idComponent = entity.AddComponent<IDComponent>(uuid);

		auto& tagComponent = entity.AddComponent<TagComponent>();
		tagComponent.Name = name.empty() ? "Empty Object" : name;

		entity.AddComponent<TransformComponent>();
		entity.AddComponent<RelationshipComponent>();

		VS_CORE_ASSERT(m_EntityMap.find(idComponent.ID) == m_EntityMap.end(), "Duplicated Entity ID in map!");
		m_EntityMap[idComponent.ID] = entity;
		return entity;
	}

	Entity Scene::CreateChildEntity(Entity parent, const std::string& name)
	{
		Entity child = CreateEntity(name);
		child.SetParent(parent);

		return child;
	}

	void Scene::ParentEntity(Entity child, Entity parent)
	{
		if (parent.IsDescendantOf(child))
		{
			UnparentEntity(parent);

			Entity newParent = TryGetEntityWithUUID(child.GetParentUUID());
			if (newParent)
			{
				UnparentEntity(child);
				ParentEntity(parent, newParent);
			}
		}
		else
		{
			Entity previousParent = TryGetEntityWithUUID(child.GetParentUUID());

			if (previousParent)
				UnparentEntity(child);
		}

		child.SetParentUUID(parent.GetUUID());
		parent.GetChildren().push_back(child.GetUUID());
		
		ConvertToLocalSpace(child);
	}

	void Scene::UnparentEntity(Entity child, bool toWorldSpace)
	{
		Entity parent = TryGetEntityWithUUID(child.GetParentUUID());
		if (!parent)
			return;

		auto& parentChildren = parent.GetChildren();
		parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), child.GetUUID()), parentChildren.end());

		if(toWorldSpace)
			ConvertToWorldSpace(child);

		child.SetParentUUID(0);
	}

	Entity Scene::GetEntityWithUUID(UUID id) const
	{
		return m_EntityMap.at(id);
	}

	Entity Scene::TryGetEntityWithUUID(UUID id) const
	{
		if (const auto iter = m_EntityMap.find(id); iter != m_EntityMap.end())
		{
			return iter->second;
		}

		return Entity();
	}

	Entity Scene::TryGetEntityWithName(const std::string& name)
	{
		auto entities = GetAllEntitiesWith<TagComponent>();

		for (auto entity : entities)
		{
			if (Entity{ entity, this }.GetName() == name)
				return Entity{ entity, this };
		}

		return Entity();
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity = CreateEntity();	
		
		CopyComponentIfExists<TagComponent>(newEntity, entity);
		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CircleRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<MeshRendererComponent>(newEntity, entity);
		CopyComponentIfExists<PointLightComponent>(newEntity, entity);
		CopyComponentIfExists<DirectionalLightComponent>(newEntity, entity);
		CopyComponentIfExists<SkyLightComponent>(newEntity, entity);
		
		//-- Build Relationship Component
		auto childrenIDs = entity.GetChildren();
		for (auto childID : childrenIDs)
		{
			Entity childDuplicated = DuplicateEntity(GetEntityWithUUID(childID));

			UnparentEntity(childDuplicated, false);

			childDuplicated.SetParentUUID(newEntity.GetUUID());
			newEntity.GetChildren().push_back(childDuplicated.GetUUID());
		}

		if (auto parent = entity.GetParent(); parent)
		{
			newEntity.SetParentUUID(parent.GetUUID());
			parent.GetChildren().push_back(newEntity.GetUUID());
		}

		return newEntity;
	}

	void Scene::DestroyEntity(Entity entity, bool destroyChildren, bool first)
	{
		if (destroyChildren)
		{
			for (size_t i = 0; i < entity.GetChildren().size(); i++)
			{
				auto childId = entity.GetChildren()[i];
				Entity child = GetEntityWithUUID(childId);
				DestroyEntity(child, destroyChildren, false);
			}
		}

		if (first)
		{
			if (auto parent = entity.GetParent(); parent)
				parent.RemoveChild(entity);
		}

		m_Registry.destroy(entity);
	}

	void Scene::SubmitToDestroyEntity(Entity entity)
	{
		SubmitPostUpdateFn([entity]() {entity.m_Scene->DestroyEntity(entity); });
	}

	void Scene::OnUpdateEditor(Ref<SceneRenderer> renderer, Timestep ts, EditorCamera& camera)
	{
		/////////////////////////////////////////////////////////////////////////////
		// LIGHTS ///////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////

		{
			m_LightEnvironment = LightEnvironment();

			// Directional Light
			{
				auto dirLights = m_Registry.view<TransformComponent, DirectionalLightComponent>();
				for (auto entity : dirLights)
				{
					auto [transfomComponent, lightComponent] = dirLights.get<TransformComponent, DirectionalLightComponent>(entity);
					glm::vec3 direction = -glm::normalize(glm::mat3(transfomComponent.GetTransform()) * glm::vec3(1.0f));
					m_LightEnvironment.DirectionalLight =
					{
						direction,
						lightComponent.Color,
						lightComponent.Intensity,
						lightComponent.CastsShadows
					};
					m_LightEnvironment.HasDirLight = true;
					m_LightEnvironment.CastsShadows = lightComponent.CastsShadows;
				}
			}
			
			// Point Lights
			{
				auto pointLights = m_Registry.view<TransformComponent, PointLightComponent>();
				m_LightEnvironment.PointLights.resize(pointLights.size());
				uint32_t index = 0;

				for (auto entity : pointLights)
				{
					auto [transformComponent, lightComponent] = pointLights.get<TransformComponent, PointLightComponent>(entity);
					auto transform = GetWorldSpaceTransform(Entity(entity, this));
					m_LightEnvironment.PointLights[index++] =
					{
						transform.Position,
						lightComponent.Intensity,
						lightComponent.Color,
						lightComponent.MinRadius,
						lightComponent.Radius,
						lightComponent.Falloff,
						lightComponent.LightSize,
						lightComponent.CastsShadows,
					};
				}
			}

			// Sky Light
			{
				auto skyLights = m_Registry.view<SkyLightComponent>();
				for (auto entity : skyLights)
				{
					auto& lightComponent = skyLights.get<SkyLightComponent>(entity);

					if (!AssetManager::IsAssetHandleValid(lightComponent.Environment))
					{
						if (lightComponent.DinamicSky)
						{
							Ref<TextureCube> preethamSky = Renderer::CreatePreethamSky(lightComponent.TurbidityAzimuthInclination);
							lightComponent.Environment = AssetManager::CreateMemoryOnlyAsset<SceneEnvironment>(preethamSky, preethamSky);
						}
					}
					
					m_LightEnvironment.SkyLight.EnvironmentMap = AssetManager::GetAsset<SceneEnvironment>(lightComponent.Environment);
					m_LightEnvironment.SkyLight.Intensity = lightComponent.Intensity;
					m_LightEnvironment.SkyLight.Lod = lightComponent.Lod;
				}
			}
		}

		renderer->BeginScene(camera);

		/////////////////////////////////////////////////////////////////////////////
		// 3D ///////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////
	
		// Models
		{
			auto view = m_Registry.view<TransformComponent, MeshRendererComponent>();
			for (auto entity : view)
			{
				auto& meshComponent = view.get<MeshRendererComponent>(entity);
				auto& materialTable = meshComponent.MaterialTable;

				glm::mat4 worldSpaceTransform = GetWorldSpaceTransformMatrix({ entity, this });

				if (AssetManager::IsAssetHandleValid(meshComponent.Model))
				{
					auto model = AssetManager::GetAsset<Model>(meshComponent.Model);
					if (!model->IsFlagSet(AssetFlag::Missing))
					{
						if (materialTable->GetMaterialCount() != model->GetMaterialTable()->GetMaterialCount())
							materialTable->SetMaterialCount(model->GetMaterialTable()->GetMaterialCount());

						if (m_EditorSelectedEntity == (uint32_t)entity)
							renderer->SubmitSelectedModel(model, materialTable, worldSpaceTransform, (int)entity);
						else
							renderer->SubmitModel(model, materialTable, worldSpaceTransform, (int)entity);
					}
				}
			}
		}
		
		/////////////////////////////////////////////////////////////////////////////
		// 2D ///////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////

		// Quads
		{
			auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
			for (auto entity : view)
			{
				auto sprite = view.get<SpriteRendererComponent>(entity);
				auto transform = GetWorldSpaceTransformMatrix({ entity, this });

				Ref<Texture2D> texture = nullptr;
				AssetMetadata texMetadata = AssetManager::GetMetadata(sprite.Texture);
				if (texMetadata.IsValid())
				{
					texture = AssetManager::GetAsset<Texture2D>(sprite.Texture);

					if (texture->IsFlagSet(AssetFlag::Invalid))
						texture = nullptr;
					else if (sprite.TextureProperties != texture->GetProperties())
						texture->SetProperties(sprite.TextureProperties, true);
				}

				renderer->SubmitQuad(transform, texture, sprite.TilingFactor, sprite.Color, (int)entity);
			}
		}

		// Circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto circle = view.get<CircleRendererComponent>(entity);
				auto transform = GetWorldSpaceTransformMatrix({ entity, this });

				renderer->SubmitCircle(transform, circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}

		renderer->EndScene();
	}

	void Scene::OnUpdateRuntime(Ref<SceneRenderer> renderer, Timestep ts)
	{
		/////////////////////////////////////////////////////////////////////////////
		// 2D PHYSICS ///////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////
		
		if (!m_IsPaused)
		{
			m_PhysicsWorld->Step(ts, m_VelocityIterations, m_PositionIterations);

			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };

				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb = entity.GetComponent<Rigidbody2DComponent>();

				b2Body* body = (b2Body*)rb.RuntimeBody;
				const auto& position = body->GetPosition();

				transform.Position.x = position.x;
				transform.Position.y = position.y;
				transform.Rotation.z = body->GetAngle();
			}
		}

		/////////////////////////////////////////////////////////////////////////////
		// SCRIPTING ////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////

		if (!m_IsPaused) 
		{
			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };

				auto& scriptComponent = entity.GetComponent<ScriptComponent>();

				if (ScriptingEngine::ModuleExists(scriptComponent.ModuleName))
				{
					ScriptingEngine::OnUpdate(ts, entity);
				}
			}

			for (auto&& fn : m_PostUpdateQueue)
				fn();
			m_PostUpdateQueue.clear();
		}

	
		/////////////////////////////////////////////////////////////////////////////
		// LIGHTS ///////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////

		{
			m_LightEnvironment = LightEnvironment();

			// Directional Light
			{
				auto dirLights = m_Registry.view<TransformComponent, DirectionalLightComponent>();
				for (auto entity : dirLights)
				{
					auto [transfomComponent, lightComponent] = dirLights.get<TransformComponent, DirectionalLightComponent>(entity);
					glm::vec3 direction = -glm::normalize(glm::mat3(transfomComponent.GetTransform()) * glm::vec3(1.0f));
					m_LightEnvironment.DirectionalLight =
					{
						direction,
						lightComponent.Color,
						lightComponent.Intensity,
						lightComponent.CastsShadows
					};
					m_LightEnvironment.HasDirLight = true;
					m_LightEnvironment.CastsShadows = lightComponent.CastsShadows;
				}
			}

			// Point Lights
			{
				auto pointLights = m_Registry.view<TransformComponent, PointLightComponent>();
				m_LightEnvironment.PointLights.resize(pointLights.size());
				uint32_t index = 0;

				for (auto entity : pointLights)
				{
					auto [transformComponent, lightComponent] = pointLights.get<TransformComponent, PointLightComponent>(entity);
					auto transform = GetWorldSpaceTransform(Entity(entity, this));
					m_LightEnvironment.PointLights[index++] =
					{
						transform.Position,
						lightComponent.Intensity,
						lightComponent.Color,
						lightComponent.MinRadius,
						lightComponent.Radius,
						lightComponent.Falloff,
						lightComponent.LightSize,
						lightComponent.CastsShadows,
					};
				}
			}

			// Sky Light
			{
				auto skyLights = m_Registry.view<SkyLightComponent>();
				for (auto entity : skyLights)
				{
					auto& lightComponent = skyLights.get<SkyLightComponent>(entity);

					if (!AssetManager::IsAssetHandleValid(lightComponent.Environment))
					{
						if (lightComponent.DinamicSky)
						{
							Ref<TextureCube> preethamSky = Renderer::CreatePreethamSky(lightComponent.TurbidityAzimuthInclination);
							lightComponent.Environment = AssetManager::CreateMemoryOnlyAsset<SceneEnvironment>(preethamSky, preethamSky);
						}
					}

					m_LightEnvironment.SkyLight.EnvironmentMap = AssetManager::GetAsset<SceneEnvironment>(lightComponent.Environment);
					m_LightEnvironment.SkyLight.Intensity = lightComponent.Intensity;
					m_LightEnvironment.SkyLight.Lod = lightComponent.Lod;
				}
			}
		}

		// Render ----------------------------------------------------------------------------------------------
		CameraComponent* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}
		if (mainCamera)
		{	
			renderer->BeginScene(*mainCamera, cameraTransform);

			/////////////////////////////////////////////////////////////////////////////
			// 3D ///////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////

			// Models
			{
				auto view = m_Registry.view<TransformComponent, MeshRendererComponent>();
				for (auto entity : view)
				{
					auto& meshComponent = view.get<MeshRendererComponent>(entity);
					auto& materialTable = meshComponent.MaterialTable;

					glm::mat4 worldSpaceTransform = GetWorldSpaceTransformMatrix({ entity, this });

					if (AssetManager::IsAssetHandleValid(meshComponent.Model))
					{
						auto model = AssetManager::GetAsset<Model>(meshComponent.Model);
						if (!model->IsFlagSet(AssetFlag::Missing))
						{
							if (materialTable->GetMaterialCount() != model->GetMaterialTable()->GetMaterialCount())
								materialTable->SetMaterialCount(model->GetMaterialTable()->GetMaterialCount());

							renderer->SubmitModel(model, materialTable, worldSpaceTransform, (int)entity);
						}
					}
				}
			}
			
			/////////////////////////////////////////////////////////////////////////////
			// 2D ///////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////

			// Quads
			{
				auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
				for (auto entity : view)
				{
					auto sprite = view.get<SpriteRendererComponent>(entity);
					auto transform = GetWorldSpaceTransformMatrix({ entity, this });

					Ref<Texture2D> texture = nullptr;
					AssetMetadata texMetadata = AssetManager::GetMetadata(sprite.Texture);
					if (texMetadata.IsValid())
					{
						texture = AssetManager::GetAsset<Texture2D>(sprite.Texture);

						if (texture->IsFlagSet(AssetFlag::Invalid))
							texture = nullptr;
						else if (sprite.TextureProperties != texture->GetProperties())
							texture->SetProperties(sprite.TextureProperties, true);
					}

					renderer->SubmitQuad(transform, texture, sprite.TilingFactor, sprite.Color, (int)entity);
				}
			}

			// Circles
			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto circle = view.get<CircleRendererComponent>(entity);
					auto transform = GetWorldSpaceTransformMatrix({ entity, this });

					renderer->SubmitCircle(transform, circle.Color, circle.Thickness, circle.Fade, (int)entity);
				}
			}

			renderer->EndScene();
		}
	}

	void Scene::OnUpdateSimulation(Ref<SceneRenderer> renderer, Timestep ts, EditorCamera& camera)
	{
		/////////////////////////////////////////////////////////////////////////////
		// 2D PHYSICS ///////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////

		if (!m_IsPaused)
		{
			m_PhysicsWorld->Step(ts, m_VelocityIterations, m_PositionIterations);

			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };

				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb = entity.GetComponent<Rigidbody2DComponent>();

				b2Body* body = (b2Body*)rb.RuntimeBody;
				const auto& position = body->GetPosition();

				transform.Position.x = position.x;
				transform.Position.y = position.y;
				transform.Rotation.z = body->GetAngle();
			}
		}

		OnUpdateEditor(renderer, ts, camera);
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}
	}

	Entity Scene::GetPrimaryCamera()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}

		return {};
	}

	glm::vec3 Scene::GetWorldSpacePosition(Entity entity)
	{
		glm::mat4 transform = entity.GetComponent<TransformComponent>().GetTransform();

		TransformComponent result;
		Math::DecomposeTransform(transform, result.Position, result.Rotation, result.Scale);

		return result.Position;
	}

	TransformComponent Scene::GetWorldSpaceTransform(Entity entity)
	{
		glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
		
		TransformComponent component;
		Math::DecomposeTransform(transform, component.Position, component.Rotation, component.Scale);

		return component;
	}

	glm::mat4 Scene::GetWorldSpaceTransformMatrix(Entity entity)
	{
		glm::mat4 transform(1.0f);

		Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());
		if (parent)
			transform = GetWorldSpaceTransformMatrix(parent);

		return transform * entity.GetComponent<TransformComponent>().GetTransform();
	}

	void Scene::ConvertToLocalSpace(Entity entity)
	{
		Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());

		if (!parent)
			return;

		auto& transform = entity.GetComponent<TransformComponent>();
		glm::mat4 parentTransform = GetWorldSpaceTransformMatrix(parent);

		glm::mat4 localTransform = glm::inverse(parentTransform) * transform.GetTransform();
		Math::DecomposeTransform(localTransform, transform.Position, transform.Rotation, transform.Scale);
	}

	void Scene::ConvertToWorldSpace(Entity entity)
	{
		Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());

		if (!parent)
			return;

		glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
		auto& entityTransform = entity.GetComponent<TransformComponent>();
		Math::DecomposeTransform(transform, entityTransform.Position, entityTransform.Rotation, entityTransform.Scale);
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity, T& component)
	{
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<RelationshipComponent>(Entity entity, RelationshipComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<MeshRendererComponent>(Entity entity, MeshRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<PointLightComponent>(Entity entity, PointLightComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<DirectionalLightComponent>(Entity entity, DirectionalLightComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SkyLightComponent>(Entity entity, SkyLightComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
	}
}