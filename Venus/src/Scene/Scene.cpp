#include "pch.h"
#include "Scene.h"

#include "Entity.h"
#include "Scripting/ScriptableEntity.h"
#include "Components.h"
#include "Renderer/Renderer2D.h"

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
		CopyComponent<NativeScriptComponent>(destRegistry, srcRegistry, enttMap);

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
		
		// Scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}
			});
		}
	}

	void Scene::OnRuntimeStop()
	{
		// Physics
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;

		// Scripts
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
		{
			if (nsc.Instance)
			{
				nsc.DestroyScript(&nsc);
			}
		});
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

		return newEntity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);
		
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

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{	
		// Physics
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

		// Scripts
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
		{
			if (nsc.Instance)
			{
				nsc.Instance->OnUpdate(ts);
			}
		});

		// Get Main Camera
		Camera* mainCamera = nullptr;
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

		// Render
		if (mainCamera)
		{	
			Renderer2D::BeginScene(*mainCamera, cameraTransform);
			
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

			Renderer2D::EndScene();
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
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}
}