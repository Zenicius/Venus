#include "pch.h"
#include "ScriptingWrappers.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/object.h>

#include "box2d/b2_body.h"

#include "Scripting/ScriptingEngine.h"
#include "Scene/Entity.h"

namespace Venus::ScriptingWrapper {

	/////////////////////////////////////////////////////////////////////////////
	// Log //////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	void Log(LogLevel level, MonoString* message)
	{
		const char* messageConverted = mono_string_to_utf8(message);

		switch (level)
		{
			case LogLevel::Trace:		LOG_TRACE(messageConverted);	break;
			case LogLevel::Info:		LOG_INFO(messageConverted);		break;
			case LogLevel::Warn:		LOG_WARN(messageConverted);		break;
			case LogLevel::Error:		LOG_ERROR(messageConverted);	break;
			case LogLevel::Critical:	LOG_CRITICAL(messageConverted); break;
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	// Input ////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	bool IsKeyPressed(KeyCode key)
	{
		return Input::IsKeyPressed(key);
	}

	bool IsMousePressed(MouseCode code)
	{
		return Input::IsMouseButtonPressed(code);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Entity ///////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	uint64_t CreateEntity()
	{
		auto scene = ScriptingEngine::GetContext();

		Entity newEntity = scene->CreateEntity("Empty Object (C#)");
		
		return newEntity.GetUUID();
	}

	void DestroyEntity(uint64_t entityID)
	{
		auto scene = ScriptingEngine::GetContext();

		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		
		scene->SubmitToDestroyEntity(entity);
	}

	uint64_t FindEntityByName(MonoString* name)
	{
		auto scene = ScriptingEngine::GetContext();
		std::string entityName = mono_string_to_utf8(name);

		Entity entity = scene->TryGetEntityWithName(entityName);

		if (entity)
			return entity.GetUUID();

		return 0;
	}

	bool ExistEntity(uint64_t entityID)
	{
		auto scene = ScriptingEngine::GetContext();

		Entity entity = scene->TryGetEntityWithUUID(entityID);

		if (entity)
			return true;

		return false;
	}

	uint64_t GetParent(uint64_t entityID)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		return entity.GetParentUUID();
	}

	void SetParent(uint64_t childID, uint64_t parentID)
	{
		Entity child = ScriptingEngine::GetContext()->GetEntityWithUUID(childID);
		Entity parent = ScriptingEngine::GetContext()->GetEntityWithUUID(parentID);

		auto scene = ScriptingEngine::GetContext();
		scene->ConvertToWorldSpace(child);
		child.SetParent(parent);
		scene->ConvertToLocalSpace(child);
	}

	bool HasParent(uint64_t childID)
	{
		Entity child = ScriptingEngine::GetContext()->GetEntityWithUUID(childID);

		return child.GetParentUUID() != 0;
	}

	MonoArray* GetChildren(uint64_t parentID)
	{
		Entity parent = ScriptingEngine::GetContext()->GetEntityWithUUID(parentID);

		const auto& children = parent.GetChildren();
		MonoArray* monoChildren = mono_array_new(mono_domain_get(), ScriptingEngine::GetMainEntityClass(), children.size());

		uint32_t index = 0;
		for (auto child : children)
		{
			void* params[1];
			params[0] = &child;

			MonoObject* obj = ScriptingEngine::NewObject("Venus.Entity:.ctor(ulong)", true, params);
			mono_array_set(monoChildren, MonoObject*, index++, obj);
		}

		return monoChildren;
	}

	void AddComponent(uint64_t entityID, void* type)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		MonoType* monoType = mono_reflection_type_get_type((MonoReflectionType*)type);

		ScriptingEngine::GetCreateComponentFunctions()[monoType](entity);
	}

	bool HasComponent(uint64_t entityID, void* type)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		MonoType* monoType = mono_reflection_type_get_type((MonoReflectionType*)type);

		return ScriptingEngine::GetHasComponentFunctions()[monoType](entity);
	}

	MonoString* GetEntityName(uint64_t entityID)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		std::string name = entity.GetName();

		return mono_string_new(mono_domain_get(), name.c_str());
	}

	void SetEntityName(uint64_t entityID, MonoString* name)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		auto& tagComponent = entity.GetComponent<TagComponent>();

		const char* newName = mono_string_to_utf8(name);

		tagComponent.Name = newName;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Transform ////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	void GetTransform(uint64_t entityID, TransformComponent* transform)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		*transform = entity.GetComponent<TransformComponent>();
	}

	void SetTransform(uint64_t entityID, TransformComponent* transform)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		entity.GetComponent<TransformComponent>() = *transform;
	}

	void GetWorldTransform(uint64_t entityID, TransformComponent* transform)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		*transform = ScriptingEngine::GetContext()->GetWorldSpaceTransform(entity);
	}

	void GetPosition(uint64_t entityID, glm::vec3* position)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		*position = entity.GetComponent<TransformComponent>().Position;
	}

	void SetPosition(uint64_t entityID, glm::vec3* position)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		entity.GetComponent<TransformComponent>().Position = *position;
	}

	void GetRotation(uint64_t entityID, glm::vec3* rotation)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		*rotation = entity.GetComponent<TransformComponent>().Rotation;
	}

	void SetRotation(uint64_t entityID, glm::vec3* rotation)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		entity.GetComponent<TransformComponent>().Rotation = *rotation;
	}

	void GetScale(uint64_t entityID, glm::vec3* scale)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		*scale = entity.GetComponent<TransformComponent>().Scale;
	}

	void SetScale(uint64_t entityID, glm::vec3* scale)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		entity.GetComponent<TransformComponent>().Scale = *scale;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Camera ///////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	bool GetIsPrimary(uint64_t entityID)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		return entity.GetComponent<CameraComponent>().Primary;
	}

	void SetIsPrimary(uint64_t entityID, bool value)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		entity.GetComponent<CameraComponent>().Primary = value;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Point Light //////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	void GetColor(uint64_t entityID, glm::vec3* color)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		*color = entity.GetComponent<PointLightComponent>().Color;
	}

	void SetColor(uint64_t entityID, glm::vec3* color)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		entity.GetComponent<PointLightComponent>().Color = *color;
	}

	float GetIntensity(uint64_t entityID)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		return entity.GetComponent<PointLightComponent>().Intensity;
	}

	void SetIntensity(uint64_t entityID, float intensity)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		entity.GetComponent<PointLightComponent>().Intensity = intensity;
	}

	/////////////////////////////////////////////////////////////////////////////
	// RigidBody2D //////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	void GetRb2DPosition(uint64_t entityID, glm::vec2* position)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);
		
		auto& rb = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb.RuntimeBody;

		glm::vec2 rbPos = glm::vec2(body->GetPosition().x, body->GetPosition().y);

		*position = rbPos;
	}

	void SetRb2DPosition(uint64_t entityID, glm::vec2* position)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);

		auto& rb = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb.RuntimeBody;

		body->SetTransform(b2Vec2(position->x, position->y), body->GetAngle());
	}

	void GetRb2DVelocity(uint64_t entityID, glm::vec2* velocity)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);

		auto& rb = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb.RuntimeBody;

		glm::vec2 rbVelocity = glm::vec2(body->GetLinearVelocity().x, body->GetLinearVelocity().y);
		
		*velocity = rbVelocity;
	}

	void SetRb2DVelocity(uint64_t entityID, glm::vec2* velocity)
	{
		Entity entity = ScriptingEngine::GetContext()->GetEntityWithUUID(entityID);

		auto& rb = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb.RuntimeBody;

		body->SetLinearVelocity(b2Vec2(velocity->x, velocity->y));
	}

	void ApplyLinearImpulse(uint64_t entityID, glm::vec2* impulse, bool wake)
	{
		auto scene = ScriptingEngine::GetContext();
		Entity entity = scene->GetEntityWithUUID(entityID);

		auto& rb = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb.RuntimeBody;

		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}
}