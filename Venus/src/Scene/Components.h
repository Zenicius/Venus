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

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

namespace Venus {

	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent 
	{
		std::string Name = "";

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& name)
			:Name(name) {}
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
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		std::string TextureName = "None";  // Editor only
		std::string TexturePath = std::string(); // Internal Only
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

		bool ShowArea = false; // Editor Only
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

		bool ShowArea = false; // Editor Only
		void* RuntimeFixture = nullptr; // Runtime Only

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	struct MeshRendererComponent 
	{
		std::string ModelName = "Cube"; // Editor Only
		std::string ModelPath = std::string(); // Internal Only

		Ref<Model> Model = Model::Create("Resources/Models/Cube.fbx");

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
	};

	struct PointLightComponent
	{
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		float LightSize = 0.5f;
		float MinRadius = 1.f;
		float Radius = 10.f;
		bool CastsShadows = true;
		bool SoftShadows = true;
		float Falloff = 1.f;
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
		std::string EnvironmentMapName = "None"; // Editor Only
		std::string EnvironmentMapPath = std::string(); // Internal Only
		float Intensity = 1.0f;
		float Lod = 0.0f;

		bool DinamicSky = false;
		glm::vec3 TurbidityAzimuthInclination = { 2.0f, 0.0f, 0.0f };

		Ref<SceneEnvironment> EnvironmentMap = nullptr;
		
		SkyLightComponent() = default;
		SkyLightComponent(const SkyLightComponent&) = default;
	};
}