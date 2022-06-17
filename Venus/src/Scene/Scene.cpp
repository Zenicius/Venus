#include "pch.h"
#include "Scene.h"

#include "Math/Math.h"

#include "Components.h"
#include "Entity.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Renderer.h"
#include "Renderer/SceneRenderer.h"

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
		CopyComponent<TransformComponent>(destRegistry, srcRegistry, enttMap);
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
					shape.SetAsBox(bc.Size.x * transform.Scale.x, bc.Size.y * transform.Scale.y);

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
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };

		entity.AddComponent<IDComponent>(uuid);

		auto& tagComponent = entity.AddComponent<TagComponent>();
		tagComponent.Name = name.empty() ? "Empty Object" : name;

		entity.AddComponent<TransformComponent>();

		return entity;
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity = CreateEntity(entity.GetName() + " Copied");

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

		return newEntity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
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
					auto position = GetWorldSpacePosition(Entity(entity, this));
					m_LightEnvironment.PointLights[index++] =
					{
						position,
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
					auto lightComponent = skyLights.get<SkyLightComponent>(entity);

					m_LightEnvironment.SkyLight.EnvironmentMap = lightComponent.EnvironmentMap;
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
				auto [transform, model] = view.get<TransformComponent, MeshRendererComponent>(entity);

				if (m_EditorSelectedEntity == (uint32_t)entity)
					renderer->SubmitSelectedModel(model.Model, transform.GetTransform(), (int)entity);
				else
					renderer->SubmitModel(model.Model, transform.GetTransform(), (int)entity);
			}
		}
		
		/////////////////////////////////////////////////////////////////////////////
		// 2D ///////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////

		// Quads
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}
		}

		// Circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}

		// Overlay
		//OnOverlayRender(camera);

		renderer->EndScene();
	}

	void Scene::OnUpdateRuntime(Ref<SceneRenderer> renderer, Timestep ts)
	{	
		/////////////////////////////////////////////////////////////////////////////
		// 2D PHYSICS ///////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////

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
					auto position = GetWorldSpacePosition(Entity(entity, this));
					m_LightEnvironment.PointLights[index++] =
					{
						position,
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
					auto lightComponent = skyLights.get<SkyLightComponent>(entity);

					m_LightEnvironment.SkyLight.EnvironmentMap = lightComponent.EnvironmentMap;
					m_LightEnvironment.SkyLight.Intensity = lightComponent.Intensity;
					m_LightEnvironment.SkyLight.Lod = lightComponent.Lod;
				}
			}
		}

		// Render ----------------------------------------------------------------------------------------------
		SceneCamera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
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
					auto [transform, model] = view.get<TransformComponent, MeshRendererComponent>(entity);
						
					renderer->SubmitModel(model.Model, transform.GetTransform(), (int)entity);
				}
			}
			
			/////////////////////////////////////////////////////////////////////////////
			// 2D ///////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////

			// Quads
			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
					Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
				}
			}

			// Circles
			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
					Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
				}
			}

			renderer->EndScene();
		}
	}

	void Scene::OnOverlayRender(EditorCamera& editorCamera)
	{
		// Selected Entity
		Entity entity = { (entt::entity)m_EditorSelectedEntity, this };
		if (entity)
		{
			// Entity Outline
			if (entity.HasComponent<MeshRendererComponent>())
			{
				const auto& meshRendererComponent = entity.GetComponent<MeshRendererComponent>();
				const auto& transformComponent = entity.GetComponent<TransformComponent>();

				//Renderer::RenderSelectedModel(meshRendererComponent.Model, transformComponent.GetTransform());
			}
			else if (entity.HasComponent<SpriteRendererComponent>())
			{
				const auto& tc = entity.GetComponent<TransformComponent>();

				glm::vec3 translation = tc.Position;
				glm::vec3 scale = { tc.Scale.x + 0.05f, tc.Scale.y + 0.05f, tc.Scale.z };

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
					* glm::toMat4(glm::quat(tc.Rotation))
					* glm::scale(glm::mat4(1.0f), scale);

				Renderer2D::DrawRect(transform, { 0.75f, 0.18f, 0.22f, 1.0f });
			}
			else if (entity.HasComponent<CircleRendererComponent>())
			{
				const auto& tc = entity.GetComponent<TransformComponent>();

				glm::vec3 translation = tc.Position;
				glm::vec3 scale = { tc.Scale.x + 0.05f, tc.Scale.y + 0.05f, tc.Scale.z };

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
					* glm::toMat4(glm::quat(tc.Rotation))
					* glm::scale(glm::mat4(1.0f), scale);

				Renderer2D::DrawCircle(transform, { 0.75f, 0.18f, 0.22f, 1.0f }, 0.05);
			}

			// Colliders Area
			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				const auto& bc = entity.GetComponent<BoxCollider2DComponent>();
				if (bc.ShowArea)
				{
					const auto& tc = entity.GetComponent<TransformComponent>();

					glm::vec3 translation = tc.Position + glm::vec3(bc.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(bc.Size * 2.0f, 1.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawRect(transform, { 0.0f, 1.0f, 0.0f, 1.0f });
				}
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				const auto& cc = entity.GetComponent<CircleCollider2DComponent>();
				if (cc.ShowArea)
				{
					float zIndex = 0.001f;
					glm::vec3 cameraForwardDirection = editorCamera.GetForwardDirection();
					glm::vec3 projectionCollider = cameraForwardDirection * glm::vec3(zIndex);

					const auto& tc = entity.GetComponent<TransformComponent>();
					
					glm::vec3 translation = tc.Position + glm::vec3(cc.Offset, -projectionCollider.z);
					glm::vec3 scale = tc.Scale * glm::vec3(cc.Radius * 2.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawCircle(transform, { 0.0f, 1.0f, 0.0f, 1.0f }, 0.05f);
				}
			}
		}
		
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

	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		auto view = m_Registry.view<IDComponent>();
		for (auto entity : view)
		{
			const auto& id = view.get<IDComponent>(entity).ID;
			
			if (uuid == id)
				return Entity{ entity, this };
		}

		return {};
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
}