#pragma once

#include "Venus.h"

class TestLayer : public Venus::Layer
{
	public:
		TestLayer();	
		virtual ~TestLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		void OnUpdate(Venus::Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Venus::Event& e) override;

	private:
		Venus::OrthographicCameraController m_CameraController;		
		Venus::ParticleSystem m_ParticleSystem;
		Venus::ParticleProps m_Particle;
		Venus::ParticleProps m_Particle2;
		Venus::ParticleProps m_Particle3;
		Venus::ParticleProps m_Particle4;
		Venus::ParticleProps m_Particle5;

		Venus::Ref<Venus::Texture2D> m_CheckerboardTexture;
		Venus::Ref<Venus::Texture2D> m_PyramidTexture;

		int m_EmitQuantity = 10;
		glm::vec2 m_CircleScale = { 19.9f, 19.9f };
		float m_CircleThickness = 1.0f;
		glm::vec3 m_QuadTransform = { 0.0f, 0.0f, 0.0f };
		glm::vec4 m_ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
};