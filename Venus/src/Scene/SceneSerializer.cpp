#include "pch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include "yaml-cpp/yaml.h"

namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}

namespace Venus {

	// Overloads to YAML work with glm types
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
	// --------------------------------------------------------------------

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		:m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		VS_CORE_ASSERT(entity.HasComponent<IDComponent>());

		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		// TagComponent
		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			auto& name = entity.GetComponent<TagComponent>().Name;
			out << YAML::Key << "Name" << YAML::Value << name;

			out << YAML::EndMap;
		}

		// TrasnformComponent
		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			auto& transformComponent = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << transformComponent.Position;
			out << YAML::Key << "Rotation" << YAML::Value << transformComponent.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transformComponent.Scale;

			out << YAML::EndMap;
		}

		// CameraComponent
		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value; 
			out << YAML::BeginMap;        // Scene Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap;		  // Scene Camera
			
			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap;
		}

		// SpriteRendererComponent
		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;
			out << YAML::Key << "TextureName" << YAML::Value << spriteRendererComponent.TextureName;
			out << YAML::Key << "Texture" << YAML::Value << spriteRendererComponent.TexturePath;
			out << YAML::Key << "TilingFactor" << YAML::Value << spriteRendererComponent.TilingFactor;

			out << YAML::EndMap;
		}

		// SpriteRendererComponent
		if (entity.HasComponent<MeshRendererComponent>())
		{
			out << YAML::Key << "MeshRendererComponent";
			out << YAML::BeginMap;

			auto& meshRendererComponent = entity.GetComponent<MeshRendererComponent>();
			out << YAML::Key << "Name" << YAML::Value << meshRendererComponent.ModelName;
			out << YAML::Key << "Path" << YAML::Value << meshRendererComponent.ModelPath;

			out << YAML::EndMap;
		}

		// CircleRendererComponent
		if (entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap;

			auto& circleRendererComponent = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.Color;
			out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.Fade;

			out << YAML::EndMap;
		}

		// Rigidbody2D Component
		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap;

			auto& rbCompenent = entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "Type" << YAML::Value << (int)rbCompenent.Type;
			out << YAML::Key << "FixedRotation" << YAML::Value << rbCompenent.FixedRotation;
			out << YAML::EndMap;
		}

		// BoxCollider2D Component
		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap;

			auto& bcComponent = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << bcComponent.Offset;
			out << YAML::Key << "Size" << YAML::Value << bcComponent.Size;
			out << YAML::Key << "Density" << YAML::Value << bcComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << bcComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << bcComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bcComponent.RestitutionThreshold;
			out << YAML::EndMap;
		}

		// CircleCollider2D Component
		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap;

			auto& ccComponent = entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << ccComponent.Offset;
			out << YAML::Key << "Radius" << YAML::Value << ccComponent.Radius;
			out << YAML::Key << "Density" << YAML::Value << ccComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << ccComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << ccComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << ccComponent.RestitutionThreshold;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;   // Entity
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << m_Scene->m_SceneName;
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq; // Entities

		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene.get() };
			if (!entity)
				return;

			SerializeEntity(out, entity);
		});

		out << YAML::EndSeq; // Entitiess
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
		fout.close();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		VS_CORE_ASSERT(false, "Not Implemented!");
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		m_Scene->m_SceneName = sceneName;
		CORE_LOG_TRACE("Loading Scene: {0}", sceneName);
		
		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				// UUID
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				// TagComponent
				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Name"].as<std::string>();

				// Entity Creation
				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

				// TransformComponent
				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					auto& transform = deserializedEntity.GetComponent<TransformComponent>();
					transform.Position = transformComponent["Position"].as<glm::vec3>();
					transform.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					transform.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				// CameraComponent
				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					auto& props = cameraComponent["Camera"];

					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)props["ProjectionType"].as<int>());

					cc.Camera.SetPerspectiveVerticalFOV(props["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(props["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(props["PerspectiveFar"].as<float>());

					cc.Camera.SetOrthographicSize(props["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(props["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(props["OrthographicFar"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				// SpriteRendererComponent
				auto srComponent = entity["SpriteRendererComponent"];
				if (srComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = srComponent["Color"].as<glm::vec4>();
					std::string texturePath = srComponent["Texture"].as<std::string>();
					src.TilingFactor = srComponent["TilingFactor"].as<float>();

					if (!texturePath.empty())
					{
						src.TextureName = srComponent["TextureName"].as<std::string>();
						src.TexturePath = texturePath;
						src.Texture = Texture2D::Create(texturePath);
					}
				}

				// MeshRendererComponent
				auto mRComponent = entity["MeshRendererComponent"];
				if (mRComponent)
				{
					auto& meshRenderer = deserializedEntity.AddComponent<MeshRendererComponent>();
					meshRenderer.ModelName = mRComponent["Name"].as<std::string>();
					std::string modelPath = mRComponent["Path"].as<std::string>();

					if (!modelPath.empty())
					{
						meshRenderer.ModelPath = modelPath;
						meshRenderer.Model = CreateRef<Model>(modelPath);
					}
					else if(meshRenderer.ModelName.compare("Sphere") == 0)
						meshRenderer.Model = Factory::CreateSphere(1.0f);
				}

				// CircleRendererComponent
				auto circleComponent = entity["CircleRendererComponent"];
				if (circleComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CircleRendererComponent>();
					cc.Color = circleComponent["Color"].as<glm::vec4>();
					cc.Thickness = circleComponent["Thickness"].as<float>();
					cc.Fade = circleComponent["Fade"].as<float>();
				}

				// Rigidbody2DComponent 
				auto rbComponent = entity["Rigidbody2DComponent"];
				if (rbComponent)
				{
					auto& rb = deserializedEntity.AddComponent<Rigidbody2DComponent>();
					rb.Type = (Rigidbody2DComponent::BodyType)rbComponent["Type"].as<int>();
					rb.FixedRotation = rbComponent["FixedRotation"].as<bool>();
				}

				// BoxCollider2DComponent 
				auto bcComponent = entity["BoxCollider2DComponent"];
				if (bcComponent)
				{
					auto& bc = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					bc.Offset = bcComponent["Offset"].as<glm::vec2>();
					bc.Size = bcComponent["Size"].as<glm::vec2>();
					bc.Density = bcComponent["Density"].as<float>();
					bc.Friction = bcComponent["Friction"].as<float>();
					bc.Restitution = bcComponent["Restitution"].as<float>();
					bc.RestitutionThreshold = bcComponent["RestitutionThreshold"].as<float>();
				}

				// CircleCollider2DComponent 
				auto ccComponent = entity["CircleCollider2DComponent"];
				if (ccComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					cc.Offset = ccComponent["Offset"].as<glm::vec2>();
					cc.Radius = ccComponent["Radius"].as<float>();
					cc.Density = ccComponent["Density"].as<float>();
					cc.Friction = ccComponent["Friction"].as<float>();
					cc.Restitution = ccComponent["Restitution"].as<float>();
					cc.RestitutionThreshold = ccComponent["RestitutionThreshold"].as<float>();
				}
			}
		}

		return true;
	}

	Entity SceneSerializer::DeserializePrefab(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			return {};
		}

		if (!data["Entity"])
			return {};

		// TagComponent
		std::string name;
		auto tagComponent = data["TagComponent"];
		if (tagComponent)
			name = tagComponent["Name"].as<std::string>();

		// Entity Creation
		Entity deserializedEntity = m_Scene->CreateEntity(name);

		// TransformComponent
		auto transformComponent = data["TransformComponent"];
		if (transformComponent)
		{
			auto& transform = deserializedEntity.GetComponent<TransformComponent>();
			transform.Rotation = transformComponent["Rotation"].as<glm::vec3>();
			transform.Scale = transformComponent["Scale"].as<glm::vec3>();
		}

		// CameraComponent
		auto cameraComponent = data["CameraComponent"];
		if (cameraComponent)
		{
			auto& cc = deserializedEntity.AddComponent<CameraComponent>();
			auto& props = cameraComponent["Camera"];

			cc.Camera.SetProjectionType((SceneCamera::ProjectionType)props["ProjectionType"].as<int>());

			cc.Camera.SetPerspectiveVerticalFOV(props["PerspectiveFOV"].as<float>());
			cc.Camera.SetPerspectiveNearClip(props["PerspectiveNear"].as<float>());
			cc.Camera.SetPerspectiveFarClip(props["PerspectiveFar"].as<float>());

			cc.Camera.SetOrthographicSize(props["OrthographicSize"].as<float>());
			cc.Camera.SetOrthographicNearClip(props["OrthographicNear"].as<float>());
			cc.Camera.SetOrthographicFarClip(props["OrthographicFar"].as<float>());

			cc.Primary = cameraComponent["Primary"].as<bool>();
			cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
		}

		// SpriteRendererComponent
		auto srComponent = data["SpriteRendererComponent"];
		if (srComponent)
		{
			auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
			src.Color = srComponent["Color"].as<glm::vec4>();
			std::string texturePath = srComponent["Texture"].as<std::string>();
			src.TilingFactor = srComponent["TilingFactor"].as<float>();

			if (!texturePath.empty())
			{
				src.TextureName = srComponent["TextureName"].as<std::string>();
				src.TexturePath = texturePath;
				src.Texture = Texture2D::Create(texturePath);
			}
		}

		// CircleRendererComponent
		auto circleComponent = data["CircleRendererComponent"];
		if (circleComponent)
		{
			auto& cc = deserializedEntity.AddComponent<CircleRendererComponent>();
			cc.Color = circleComponent["Color"].as<glm::vec4>();
			cc.Thickness = circleComponent["Thickness"].as<float>();
			cc.Fade = circleComponent["Fade"].as<float>();
		}

		// Rigidbody2DComponent 
		auto rbComponent = data["Rigidbody2DComponent"];
		if (rbComponent)
		{
			auto& rb = deserializedEntity.AddComponent<Rigidbody2DComponent>();
			rb.Type = (Rigidbody2DComponent::BodyType)rbComponent["Type"].as<int>();
			rb.FixedRotation = rbComponent["FixedRotation"].as<bool>();
		}

		// BoxCollider2DComponent 
		auto bcComponent = data["BoxCollider2DComponent"];
		if (bcComponent)
		{
			auto& bc = deserializedEntity.AddComponent<BoxCollider2DComponent>();
			bc.Offset = bcComponent["Offset"].as<glm::vec2>();
			bc.Size = bcComponent["Size"].as<glm::vec2>();
			bc.Density = bcComponent["Density"].as<float>();
			bc.Friction = bcComponent["Friction"].as<float>();
			bc.Restitution = bcComponent["Restitution"].as<float>();
			bc.RestitutionThreshold = bcComponent["RestitutionThreshold"].as<float>();
		}

		// CircleCollider2DComponent 
		auto ccComponent = data["CircleCollider2DComponent"];
		if (ccComponent)
		{
			auto& cc = deserializedEntity.AddComponent<CircleCollider2DComponent>();
			cc.Offset = ccComponent["Offset"].as<glm::vec2>();
			cc.Radius = ccComponent["Radius"].as<float>();
			cc.Density = ccComponent["Density"].as<float>();
			cc.Friction = ccComponent["Friction"].as<float>();
			cc.Restitution = ccComponent["Restitution"].as<float>();
			cc.RestitutionThreshold = ccComponent["RestitutionThreshold"].as<float>();
		}

		return deserializedEntity;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		VS_CORE_ASSERT(false, "Not Implemented!");
		return false;
	}

}