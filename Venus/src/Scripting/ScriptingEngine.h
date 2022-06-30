#pragma once

#include "Scene/Scene.h"

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoType MonoType;
}

namespace Venus {

	class ScriptingEngine
	{
		public:
			static void Init();
			static void Shutdown();

			static void Reload();
		
			static void SetContext(Scene* scene);
			static Scene* GetContext();
			static MonoClass* GetMainEntityClass();

			static bool ModuleExists(const std::string& moduleName, bool checkSubClassEntity = true);
			static bool IsSubclassOfEntity(MonoClass* subclass);
			static bool IsSubclassOfEntity(const std::string& moduleName);

			static MonoObject* NewObject(const std::string& fullName, bool construct = true, void** params = nullptr);
			static void Instantiate(Entity entity);
			static void ClearEntityData();

			static void OnCreate(Entity entity);
			static void OnUpdate(float Timestep, Entity entity);

			static void RegisterFunctions();
			static std::unordered_map<MonoType*, std::function<void(Entity&)>> GetCreateComponentFunctions();
			static std::unordered_map<MonoType*, std::function<bool(Entity&)>> GetHasComponentFunctions();
	};

}

