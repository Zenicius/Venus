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

			//-- Component Functions-------------------------------------------------------------------------------------
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
			const T& GetComponent() const
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
			bool HasComponent() const
			{
				return m_Scene->m_Registry.has<T>(m_EntityHandle);
			}

			template<typename T>
			void RemoveComponent()
			{
				VS_CORE_ASSERT(HasComponent<T>(), "Entity doesnt have component!");
				m_Scene->m_Registry.remove<T>(m_EntityHandle);
			}
			//-----------------------------------------------------------------------------------------------------------


			//-- Relationship Functions----------------------------------------------------------------------------------
			void SetParentUUID(UUID parent) { GetComponent<RelationshipComponent>().Parent = parent; }
			UUID GetParentUUID() const { return GetComponent<RelationshipComponent>().Parent; }
			std::vector<UUID>& GetChildren() { return GetComponent<RelationshipComponent>().Children; }

			Entity GetParent()
			{
				return m_Scene->TryGetEntityWithUUID(GetParentUUID());
			}

			void SetParent(Entity parent)
			{
				Entity currentParent = GetParent();
				if (currentParent == parent)
					return;

				// Changing parent, remove from the child vector of the anterior
				if (currentParent)
					currentParent.RemoveChild(*this);

				SetParentUUID(parent.GetUUID());

				if (parent)
				{
					auto& parentChildren = parent.GetChildren();
					UUID id = GetUUID();
					if (std::find(parentChildren.begin(), parentChildren.end(), id) == parentChildren.end())
						parentChildren.emplace_back(GetUUID());
				}
			}

			bool IsAncestorOf(Entity entity)
			{
				const auto& children = GetChildren();

				if (children.empty())
					return false;

				for (UUID childID : children)
				{
					if (childID == entity.GetUUID())
						return true;
				}

				for (UUID childID : children)
				{
					if (m_Scene->GetEntityWithUUID(childID).IsAncestorOf(entity))
						return true;
				}

				return false;
			}

			bool IsDescendantOf(Entity entity)
			{
				return entity.IsAncestorOf(*this);
			}

			bool RemoveChild(Entity child)
			{
				UUID childId = child.GetUUID();
				std::vector<UUID>& children = GetChildren();
				auto it = std::find(children.begin(), children.end(), childId);
				if (it != children.end())
				{
					children.erase(it);
					return true;
				}

				return false;
			}
			//-----------------------------------------------------------------------------------------------------------
			
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

			friend class Scene;
	};
}
