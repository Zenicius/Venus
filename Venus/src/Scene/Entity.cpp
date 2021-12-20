#include "pch.h"
#include "Entity.h"

namespace Venus {

	Entity::Entity(entt::entity handle, Scene* scene)
		:m_EntityHandle(handle), m_Scene(scene)
	{
	}
}