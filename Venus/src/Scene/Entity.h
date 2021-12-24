#pragma once

#include "Engine/UUID.h"
#include "Scene.h"
#include "Components.h"

#include "entt.hpp"

namespace Venus {

	class Entity
	{
		public:
			Entity() = default;
			Entity(entt::entity handle, Scene* scene);

			template<typename T, typename... Args>
			T& AddComponent(Args&&... args)
			{
				VS_CORE_ASSERT(!HasComponent<T>(), "Entity Already has component!");
				T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
				m_Scene->OnComponentAdded<T>(*this, component);
				return component;
			}

			template<typename T, typename... Args>
			T& AddOrReplaceComponent(Args&&... args)
			{
				T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
				m_Scene->OnComponentAdded<T>(*this, component);
				return component;
			}

			template<typename T>
			T& GetComponent()
			{
				VS_CORE_ASSERT(HasComponent<T>(), "Entity doesnt have component!");
				return m_Scene->m_Registry.get<T>(m_EntityHandle);
			}

			template<typename T>
			bool HasComponent()
			{
				return m_Scene->m_Registry.has<T>(m_EntityHandle);
			}

			template<typename T>
			void RemoveComponent()
			{
				VS_CORE_ASSERT(HasComponent<T>(), "Entity doesnt have component!");
				m_Scene->m_Registry.remove<T>(m_EntityHandle);
			}

			operator bool() const { return m_EntityHandle != entt::null; }
			operator entt::entity() const { return m_EntityHandle; }
			operator uint32_t() const { return (uint32_t) m_EntityHandle; }

			bool operator==(const Entity& other) const
			{
				return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
			}

			bool operator!=(const Entity& other) const
			{
				return !operator==(other);
			}

			UUID GetUUID() { return GetComponent<IDComponent>().ID; }
			std::string GetName() { return GetComponent<TagComponent>().Name; }

		private:
			entt::entity m_EntityHandle{ entt::null };
			Scene* m_Scene = nullptr; // Temp
	};
}
