#pragma once

#include "Engine/Input.h"
#include "Scene/Components.h"

extern "C" {
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoString MonoString;
	typedef struct _MonoArray MonoArray;
}

namespace Venus::ScriptingWrapper {

	// Log
	enum class LogLevel
	{
		Trace = 0,
		Info = 1,
		Warn = 2,
		Error = 3,
		Critical = 4
	};
	void Log(LogLevel level, MonoString* message);

	// Input
	bool IsKeyPressed(KeyCode key);
	bool IsMousePressed(MouseCode code);

	// Entity
	uint64_t CreateEntity();
	void DestroyEntity(uint64_t entityID);

	uint64_t FindEntityByName(MonoString* name);
	bool ExistEntity(uint64_t entityID);

	uint64_t GetParent(uint64_t entityID);
	void SetParent(uint64_t childID, uint64_t parentID);
	bool HasParent(uint64_t childID);
	MonoArray* GetChildren(uint64_t parentID);

	void AddComponent(uint64_t entityID, void* type);
	bool HasComponent(uint64_t entityID, void* type);

	MonoString* GetEntityName(uint64_t entityID);
	void SetEntityName(uint64_t entityID, MonoString* name);

	void GetTransform(uint64_t entityID, TransformComponent* transform);
	void SetTransform(uint64_t entityID, TransformComponent* transform);
	void GetWorldTransform(uint64_t entityID, TransformComponent* transform);

	void GetPosition(uint64_t entityID, glm::vec3* position);
	void SetPosition(uint64_t entityID, glm::vec3* position);
	void GetRotation(uint64_t entityID, glm::vec3* rotation);
	void SetRotation(uint64_t entityID, glm::vec3* rotation);
	void GetScale(uint64_t entityID, glm::vec3* scale);
	void SetScale(uint64_t entityID, glm::vec3* scale);

	bool GetIsPrimary(uint64_t entityID);
	void SetIsPrimary(uint64_t entityID, bool value);

	void GetColor(uint64_t entityID, glm::vec3* color);
	void SetColor(uint64_t entityID, glm::vec3* color);
	float GetIntensity(uint64_t entityID);
	void SetIntensity(uint64_t entityID, float intensity);

}

