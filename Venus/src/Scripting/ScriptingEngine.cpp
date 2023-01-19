#include "pch.h"
#include "ScriptingEngine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/object.h>

#include "ScriptingWrappers.h"

#include "Scene/Entity.h"

namespace Venus {

	static std::string binariesPath = "assets/scripting/Dev/Binaries/";

	static Scene* s_Context = nullptr;
	static MonoDomain* s_MonoDomain = nullptr;
	static MonoDomain* s_NewDomain = nullptr;

	//--Venus
	static MonoAssembly* s_VenusAssembly = nullptr;
	static MonoImage* s_VenusImage = nullptr;
	static MonoClass* s_EntityMainClass = nullptr;
	static MonoMethod* s_ExceptionMethod = nullptr;
	//--Client
	static MonoAssembly* s_ClientAssembly = nullptr;
	static MonoImage* s_ClientImage = nullptr;

	// Entity
	struct EntityScriptingData
	{
		MonoClass* Class = nullptr;
		MonoObject* Instance = nullptr;

		// Methods
		MonoMethod* ConstructorMethod = nullptr;
		MonoMethod* CreateMethod = nullptr;
		MonoMethod* UpdateMethod = nullptr;
	};
	static std::unordered_map<UUID, Ref<EntityScriptingData>> s_EntityDataMap;

	// Component Functions
	static std::unordered_map<MonoType*, std::function<void(Entity&)>> s_CreateComponentFunctions;
	static std::unordered_map<MonoType*, std::function<bool(Entity&)>> s_HasComponentFunctions;

	MonoAssembly* LoadAssembly(const char* path)
	{
		if (path == NULL)
		{
			return NULL;
		}

		HANDLE file = CreateFileA(path, 0x0001, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file == INVALID_HANDLE_VALUE)
		{
			return NULL;
		}

		DWORD file_size = GetFileSize(file, NULL);
		if (file_size == INVALID_FILE_SIZE)
		{
			CloseHandle(file);
			return NULL;
		}

		void* file_data = malloc(file_size);
		if (file_data == NULL)
		{
			CloseHandle(file);
			return NULL;
		}

		DWORD read = 0;
		bool readSucess = ReadFile(file, file_data, file_size, &read, NULL);
		if (file_size != read)
		{
			free(file_data);
			CloseHandle(file);
			return NULL;
		}

		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(reinterpret_cast<char*>(file_data), file_size, 1, &status, 0);
		if (status != MONO_IMAGE_OK)
		{
			return NULL;
		}
		auto assemb = mono_assembly_load_from_full(image, path, &status, 0);
		free(file_data);
		CloseHandle(file);
		mono_image_close(image);
		return assemb;
	}

	static MonoMethod* GetMethod(const std::string& name, MonoImage* image)
	{
		MonoMethodDesc* methodDesc = mono_method_desc_new(name.c_str(), NULL);
		MonoMethod* method = mono_method_desc_search_in_image(methodDesc, image);

		if (!method)
			return nullptr;

		return method;
	}

	static MonoObject* InvokeMethod(MonoObject* object, MonoMethod* method, void** params = nullptr)
	{
		MonoObject* exception = nullptr;
		MonoObject* result = mono_runtime_invoke(method, object, params, &exception);

		if (exception)
		{
			void* args[1];
			args[0] = exception;
			MonoObject* result = mono_runtime_invoke(s_ExceptionMethod, NULL, args, NULL);
		}

		return result;
	}

	void ScriptingEngine::Init()
	{
		//- Init Mono-----------------------
		mono_set_assemblies_path("mono/lib");
		auto domain = mono_jit_init("VenusScriptingEngine");
		
		//- Init Venus Assembly-------------
		s_MonoDomain = mono_domain_create_appdomain("Venus Runtime", nullptr);
		mono_domain_set(s_MonoDomain, false);

		s_VenusAssembly = LoadAssembly((binariesPath + "VenusScripting.dll").c_str());
		s_VenusImage = mono_assembly_get_image(s_VenusAssembly);

		s_EntityMainClass = mono_class_from_name(s_VenusImage, "Venus", "Entity");
		s_ExceptionMethod = GetMethod("Venus.RuntimeException:OnException(object)", s_VenusImage);

		//- Init Client Assembly------------
		s_ClientAssembly = LoadAssembly((binariesPath + "Dev.dll").c_str());
		s_ClientImage = mono_assembly_get_image(s_ClientAssembly);

		if (!s_VenusAssembly || !s_VenusImage || !s_ClientAssembly || !s_ClientImage || !s_EntityMainClass || !s_ExceptionMethod)
		{
			CORE_LOG_CRITICAL("Failed to initialize Scripting Engine!");
			return;
		}

		CORE_LOG_INFO("Initialized Scripting Engine!");
		RegisterFunctions();
	}

	void ScriptingEngine::Shutdown()
	{
		s_EntityDataMap.clear();
		mono_jit_cleanup(s_MonoDomain);
	}

	void ScriptingEngine::Reload()
	{
		s_NewDomain = mono_domain_create_appdomain("Venus Runtime", nullptr);
		mono_domain_set(s_NewDomain, false);

		MonoAssembly* assemblyToUnload = s_VenusAssembly;

		//- Init Venus Assembly-------------
		s_VenusAssembly = LoadAssembly((binariesPath + "VenusScripting.dll").c_str());
		s_VenusImage = mono_assembly_get_image(s_VenusAssembly);

		s_EntityMainClass = mono_class_from_name(s_VenusImage, "Venus", "Entity");
		s_ExceptionMethod = GetMethod("Venus.RuntimeException:OnException(object)", s_VenusImage);

		//- Init Client Assembly------------
		s_ClientAssembly = LoadAssembly((binariesPath + "Dev.dll").c_str());
		s_ClientImage = mono_assembly_get_image(s_ClientAssembly);

		//- Cleanup-------------------------
		mono_domain_unload(s_MonoDomain);
		assemblyToUnload = nullptr; // ???
		s_MonoDomain = s_NewDomain;
		s_NewDomain = nullptr;

		if (!s_VenusAssembly || !s_VenusImage || !s_ClientAssembly || !s_ClientImage || !s_EntityMainClass || !s_ExceptionMethod)
		{
			CORE_LOG_CRITICAL("Failed to reload assemblies!");
			return;
		}

		CORE_LOG_INFO("Reloaded Assemblies!");
		RegisterFunctions();
	}

	void ScriptingEngine::SetContext(Scene* scene)
	{
		s_Context = scene;
	}

	Scene* ScriptingEngine::GetContext()
	{
		return s_Context;
	}

	MonoClass* ScriptingEngine::GetMainEntityClass()
	{
		return s_EntityMainClass;
	}

	bool ScriptingEngine::ModuleExists(const std::string& moduleName, bool checkSubClassEntity)
	{
		if (!s_ClientAssembly)
			return false;

		std::string namespaceName, className;
		namespaceName = moduleName.substr(0, moduleName.find_last_of('.'));
		className = moduleName.substr(moduleName.find_last_of('.') + 1);

		MonoClass* monoClass = mono_class_from_name(s_ClientImage, namespaceName.c_str(), className.c_str());
		if (!monoClass)
			return false;

		if(checkSubClassEntity)
			return IsSubclassOfEntity(monoClass);

		return true;
	}

	bool ScriptingEngine::IsSubclassOfEntity(MonoClass* subclass)
	{
		bool isSubclass = mono_class_is_subclass_of(subclass, s_EntityMainClass, 0);

		return isSubclass;
	}

	bool ScriptingEngine::IsSubclassOfEntity(const std::string& moduleName)
	{
		std::string namespaceName, className;
		namespaceName = moduleName.substr(0, moduleName.find_last_of('.'));
		className = moduleName.substr(moduleName.find_last_of('.') + 1);

		MonoClass* subclass = mono_class_from_name(s_ClientImage, namespaceName.c_str(), className.c_str());
		
		if (subclass)
			return IsSubclassOfEntity(subclass);

		return false;
	}

	MonoObject* ScriptingEngine::NewObject(const std::string& fullName, bool construct, void** params)
	{
		std::string namespaceName, className, constructorName;
		namespaceName = fullName.substr(0, fullName.find_first_of('.'));
		className = fullName.substr(fullName.find_first_of('.') + 1, (fullName.find_first_of(':') - fullName.find_first_of('.')) - 1);
		constructorName = fullName.substr(fullName.find_first_of(':'));

		MonoClass* monoClass = mono_class_from_name(s_VenusImage, namespaceName.c_str(), className.c_str());

		VS_CORE_ASSERT(monoClass, "Class not found!");

		MonoObject* obj = mono_object_new(s_MonoDomain, monoClass);

		if (construct)
		{
			MonoMethodDesc* desc = mono_method_desc_new(constructorName.c_str(), NULL);
			MonoMethod* constructor = mono_method_desc_search_in_class(desc, monoClass);

			VS_CORE_ASSERT(constructor, "Constructor not found!");

			InvokeMethod(obj, constructor, params);
		}

		return obj;
	}

	void ScriptingEngine::Instantiate(Entity entity)
	{
		std::string moduleName = entity.GetComponent<ScriptComponent>().ModuleName;

		if (moduleName.empty() || !ModuleExists(moduleName))
			return;

		std::string namespaceName, className;
		namespaceName = moduleName.substr(0, moduleName.find_last_of('.'));
		className = moduleName.substr(moduleName.find_last_of('.') + 1);

		Ref<EntityScriptingData> data = CreateRef<EntityScriptingData>();
		data->Class = mono_class_from_name(s_ClientImage, namespaceName.c_str(), className.c_str());
		data->Instance = mono_object_new(s_MonoDomain, data->Class);

		std::string constructorDesc = "Venus.Entity:.ctor(ulong)";
		std::string createDesc = moduleName + ":Start()";
		std::string updateDesc = moduleName + ":Update(single)";
		data->ConstructorMethod = GetMethod(constructorDesc, s_VenusImage);
		// Instantiate and construct
		{
			mono_runtime_object_init(data->Instance);
			
			void* args[1];
			args[0] = &entity.GetUUID();

			InvokeMethod(data->Instance, data->ConstructorMethod, args);
		}

		data->CreateMethod = GetMethod(createDesc, s_ClientImage);
		data->UpdateMethod = GetMethod(updateDesc, s_ClientImage);

		if (!data->CreateMethod)
			LOG_WARN("Could not find Start Method in: {0}", className);

		if(!data->UpdateMethod)
			LOG_WARN("Could not find Update Method in: {0}", className);

		auto id = entity.GetUUID();
		
		s_EntityDataMap[id] = data;
	}

	void ScriptingEngine::ClearEntityData()
	{
		s_EntityDataMap.clear();
	}

	void ScriptingEngine::OnCreate(Entity entity)
	{
		auto id = entity.GetUUID();

		auto& data = s_EntityDataMap[id];

		if (data->CreateMethod)
		{
			MonoObject* result = InvokeMethod(data->Instance, data->CreateMethod);
		}
	}

	void ScriptingEngine::OnUpdate(float Timestep, Entity entity)
	{
		auto id = entity.GetUUID();

		auto& data = s_EntityDataMap[id];

		if (data->UpdateMethod)
		{
			void* args[1];
			args[0] = &Timestep;

			MonoObject* result = InvokeMethod(data->Instance, data->UpdateMethod, args);
		}
	}

	void ScriptingEngine::RegisterFunctions()
	{
		//-- Reflect Component Types------------------------------------------------------------------------------
		MonoType* type = mono_reflection_type_from_name("Venus.TagComponent", s_VenusImage);
		s_CreateComponentFunctions[type] = [](Entity& entity) { return entity.AddComponent<TagComponent>(); };
		s_HasComponentFunctions[type] = [](Entity& entity) { return entity.HasComponent<TagComponent>(); };

		type = mono_reflection_type_from_name("Venus.TransformComponent", s_VenusImage);
		s_CreateComponentFunctions[type] = [](Entity& entity) { return entity.AddComponent<TransformComponent>(); };
		s_HasComponentFunctions[type] = [](Entity& entity) { return entity.HasComponent<TransformComponent>(); };

		type = mono_reflection_type_from_name("Venus.CameraComponent", s_VenusImage);
		s_CreateComponentFunctions[type] = [](Entity& entity) { return entity.AddComponent<CameraComponent>(); };
		s_HasComponentFunctions[type] = [](Entity& entity) { return entity.HasComponent<CameraComponent>(); };

		type = mono_reflection_type_from_name("Venus.PointLightComponent", s_VenusImage);
		s_CreateComponentFunctions[type] = [](Entity& entity) { return entity.AddComponent<PointLightComponent>(); };
		s_HasComponentFunctions[type] = [](Entity& entity) { return entity.HasComponent<PointLightComponent>(); };

		type = mono_reflection_type_from_name("Venus.RigidBody2DComponent", s_VenusImage);
		s_CreateComponentFunctions[type] = [](Entity& entity) { return entity.AddComponent<Rigidbody2DComponent>(); };
		s_HasComponentFunctions[type] = [](Entity& entity) { return entity.HasComponent<Rigidbody2DComponent>(); };

		//--------------------------------------------------------------------------------------------------------

		/////////////////////////////////////////////////////////////////////////////
		// Entity ///////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////

		// Management
		mono_add_internal_call("Venus.Entity::CreateEntity_VenusEngine", ScriptingWrapper::CreateEntity);
		mono_add_internal_call("Venus.Entity::DestroyEntity_VenusEngine", ScriptingWrapper::DestroyEntity);
		mono_add_internal_call("Venus.Entity::FindEntityByName_VenusEngine", ScriptingWrapper::FindEntityByName);
		mono_add_internal_call("Venus.Entity::ExistEntity_VenusEngine", ScriptingWrapper::ExistEntity);

		// Relationship
		mono_add_internal_call("Venus.Entity::GetParent_VenusEngine", ScriptingWrapper::GetParent);
		mono_add_internal_call("Venus.Entity::SetParent_VenusEngine", ScriptingWrapper::SetParent);
		mono_add_internal_call("Venus.Entity::HasParent_VenusEngine", ScriptingWrapper::HasParent);
		mono_add_internal_call("Venus.Entity::GetChildren_VenusEngine", ScriptingWrapper::GetChildren);

		// Component Functions
		mono_add_internal_call("Venus.Entity::AddComponent_VenusEngine", ScriptingWrapper::AddComponent);
		mono_add_internal_call("Venus.Entity::HasComponent_VenusEngine", ScriptingWrapper::HasComponent);
		
		// TagComponent
		mono_add_internal_call("Venus.TagComponent::GetEntityName_VenusEngine", ScriptingWrapper::GetEntityName);
		mono_add_internal_call("Venus.TagComponent::SetEntityName_VenusEngine", ScriptingWrapper::SetEntityName);

		// TransformComponent
		mono_add_internal_call("Venus.TransformComponent::GetTransform_VenusEngine", ScriptingWrapper::GetTransform);
		mono_add_internal_call("Venus.TransformComponent::SetTransform_VenusEngine", ScriptingWrapper::SetTransform);
		mono_add_internal_call("Venus.TransformComponent::GetWorldTransform_VenusEngine", ScriptingWrapper::GetWorldTransform);

		mono_add_internal_call("Venus.TransformComponent::GetPosition_VenusEngine", ScriptingWrapper::GetPosition);
		mono_add_internal_call("Venus.TransformComponent::SetPosition_VenusEngine", ScriptingWrapper::SetPosition);
		mono_add_internal_call("Venus.TransformComponent::GetRotation_VenusEngine", ScriptingWrapper::GetRotation);
		mono_add_internal_call("Venus.TransformComponent::SetRotation_VenusEngine", ScriptingWrapper::SetRotation);
		mono_add_internal_call("Venus.TransformComponent::GetScale_VenusEngine", ScriptingWrapper::GetScale);
		mono_add_internal_call("Venus.TransformComponent::SetScale_VenusEngine", ScriptingWrapper::SetScale);

		// CameraComponent
		mono_add_internal_call("Venus.CameraComponent::GetIsPrimary_VenusEngine", ScriptingWrapper::GetIsPrimary);
		mono_add_internal_call("Venus.CameraComponent::SetIsPrimary_VenusEngine", ScriptingWrapper::SetIsPrimary);

		// PointLightComponent
		mono_add_internal_call("Venus.PointLightComponent::GetColor_VenusEngine", ScriptingWrapper::GetColor);
		mono_add_internal_call("Venus.PointLightComponent::SetColor_VenusEngine", ScriptingWrapper::SetColor);
		mono_add_internal_call("Venus.PointLightComponent::GetIntensity_VenusEngine", ScriptingWrapper::GetIntensity);
		mono_add_internal_call("Venus.PointLightComponent::SetIntensity_VenusEngine", ScriptingWrapper::SetIntensity);

		// RigidBody2D
		mono_add_internal_call("Venus.RigidBody2DComponent::ApplyLinearImpulse_VenusEngine", ScriptingWrapper::ApplyLinearImpulse);
		mono_add_internal_call("Venus.RigidBody2DComponent::GetRb2DPosition_VenusEngine", ScriptingWrapper::GetRb2DPosition);
		mono_add_internal_call("Venus.RigidBody2DComponent::SetRb2DPosition_VenusEngine", ScriptingWrapper::SetRb2DPosition);
		mono_add_internal_call("Venus.RigidBody2DComponent::GetRb2DVelocity_VenusEngine", ScriptingWrapper::GetRb2DVelocity);
		mono_add_internal_call("Venus.RigidBody2DComponent::SetRb2DVelocity_VenusEngine", ScriptingWrapper::SetRb2DVelocity);

		/////////////////////////////////////////////////////////////////////////////
		// Log //////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////
		mono_add_internal_call("Venus.Log::Log_VenusEngine", ScriptingWrapper::Log);

		/////////////////////////////////////////////////////////////////////////////
		// Input ////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////
		mono_add_internal_call("Venus.Input::IsMousePressed_VenusEngine", ScriptingWrapper::IsMousePressed);
		mono_add_internal_call("Venus.Input::IsKeyPressed_VenusEngine", ScriptingWrapper::IsKeyPressed);
	}

	std::unordered_map<MonoType*, std::function<void(Entity&)>> ScriptingEngine::GetCreateComponentFunctions()
	{
		return s_CreateComponentFunctions;
	}

	std::unordered_map<MonoType*, std::function<bool(Entity&)>> ScriptingEngine::GetHasComponentFunctions()
	{
		return s_HasComponentFunctions;
	}

}