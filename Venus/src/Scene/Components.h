#pragma once

#include "Engine/Base.h"
#include "Engine/UUID.h"
#include "Scene/Factory.h"
#include "Scene/SceneCamera.h"
#include "Scene/SceneEnvironment.h"
#include "Renderer/Renderer.h"
#include "Renderer/Texture.h"
#include "Renderer/Mesh.h"
#include "Renderer/MeshMaterial.h"

#include "ImGui/IconsFontAwesome.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

namespace Venus {

	struct IDComponent
	{
		UUID ID = 0;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	enum class TagIcon
	{
		Empty = 0,
		Folder = 1,
		Camera = 2,
		Model = 3,
		Sprite = 4,
		Light = 5
	};
	struct TagComponent 
	{
		std::string Name = "";
		TagIcon Icon = TagIcon::Empty;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& name)
			:Name(name) {}

		std::string GetIconString()
		{
			switch (Icon)
			{
				case TagIcon::Empty:	return ICON_FA_FILE; 
				case TagIcon::Folder:	return ICON_FA_FOLDER; 
				case TagIcon::Camera:	return ICON_FA_VIDEO_CAMERA; 
				case TagIcon::Model:	return ICON_FA_CUBE; 
				case TagIcon::Sprite:	return ICON_FA_FILE_IMAGE_O; 
				case TagIcon::Light:	return ICON_FA_LIGHTBULB_O; 
			}
		}

		std::string ToStringIcon()
		{
			switch (Icon)
			{
				case TagIcon::Empty:	return "Empty"; 
				case TagIcon::Folder:	return "Folder"; 
				case TagIcon::Camera:	return "Camera"; 
				case TagIcon::Model:	return "Model"; 
				case TagIcon::Sprite:	return "Sprite"; 
				case TagIcon::Light:	return "Light"; 
			}
		}
	};

	struct RelationshipComponent
	{
		UUID Parent = 0;
		std::vector<UUID> Children;

		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent& other) = default;
		RelationshipComponent(UUID parent)
			: Parent(parent) {}
	};

	struct TransformComponent
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& positionn)
			: Position(positionn) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Position) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct SpriteRendererComponent
	{
		AssetHandle Texture;
		TextureProperties TextureProperties;
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct CircleRendererComponent
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
		CircleRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};
	
	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;
		glm::vec4 BackgroundColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		// Post
		bool UseRendererSettings = true;

		bool Bloom = true;
		float BloomIntensity = 1.0f;
		AssetHandle BloomDirtMask;
		float BloomDirtMaskIntensity = 1.0f;

		float Exposure = 1.0f;
		bool ACESTone = true;
		bool GammaCorrection = true;
		bool Grayscale = false;


		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct Rigidbody2DComponent
	{
		enum class BodyType
		{
			Static = 0,
			Dynamic = 1,
			Kinematic = 2
		};

		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		void* RuntimeBody = nullptr; // Runtime Only

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f }; // 1x1 Sprite

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;	

		void* RuntimeFixture = nullptr; // Runtime Only

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		float Radius = 0.5f;

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		void* RuntimeFixture = nullptr; // Runtime Only

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	struct MeshRendererComponent 
	{
		AssetHandle Model;
		Ref<MaterialTable> MaterialTable = CreateRef<Venus::MaterialTable>();

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent& other)
			: Model(other.Model), MaterialTable(CreateRef<Venus::MaterialTable>(other.MaterialTable))
		{
		}
	};

	struct PointLightComponent
	{
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		float LightSize = 0.5f;
		float MinRadius = 1.0f;
		float Radius = 10.f;
		bool CastsShadows = true;
		bool SoftShadows = true;
		float Falloff = 1.0f;
	};

	struct DirectionalLightComponent
	{
		glm::vec3 Color{ 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		bool CastsShadows = true;
		bool SoftShadows = true;
		float LightSize = 0.5f;
	};

	struct SkyLightComponent
	{
		AssetHandle Environment;

		float Intensity = 1.0f;
		float Lod = 0.0f;

		bool DinamicSky = false;
		glm::vec3 TurbidityAzimuthInclination = { 2.0f, 0.0f, 0.0f };
		
		SkyLightComponent() = default;
		SkyLightComponent(const SkyLightComponent&) = default;
	};

	struct ScriptComponent
	{
		std::string ModuleName = "";

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent& other) = default;
		ScriptComponent(const std::string& moduleName)
			: ModuleName(moduleName) {}
	};
}