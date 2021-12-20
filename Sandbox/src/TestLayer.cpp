#include "TestLayer.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>

TestLayer::TestLayer()
	: Layer("TestLayer"), m_CameraController(1600.0f / 900.0f)
{
}

void TestLayer::OnAttach() 
{
	VS_PROFILE_FUNCTION();

	// Textures
	m_CheckerboardTexture = Venus::Texture2D::Create("assets/textures/Checkerboard.png");
	m_PyramidTexture = Venus::Texture2D::Create("assets/textures/pyramid.jpg");

	// Particle
	m_ParticleSystem.SetPoolSize(70000);

	m_Particle.ColorBegin = { 1.0f, 0.0f, 0.0f, 1.0f };
	m_Particle.ColorEnd = { 0.0f, 0.0f, 1.0f, 0.0f };
	m_Particle.SizeBegin = 0.2f, m_Particle.SizeVariation = 0.1f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };

	m_Particle2.Position = { 0.0, -2.0f };
	m_Particle2.ColorBegin = { 0.0f, 0.0f, 1.0f, 1.0f };
	m_Particle2.ColorEnd = { 1.0f, 1.0f, 1.0f, 0.0f };
	m_Particle2.SizeBegin = 0.2f, m_Particle2.SizeVariation = 0.1f, m_Particle2.SizeEnd = 0.0f;
	m_Particle2.LifeTime = 1.0f;
	m_Particle2.Velocity = { 0.0f, -1.0f };
	m_Particle2.VelocityVariation = { 3.0f, 1.0f };
	m_Particle2.Position = { 0.0f, 0.0f };

	m_Particle3.Position = { 0.0, 2.0f };
	m_Particle3.ColorBegin = { 1.0f, 0.0f, 0.0f, 1.0f };
	m_Particle3.ColorEnd = { 1.0f, 1.0f, 1.0f, 0.0f };
	m_Particle3.SizeBegin = 0.2f, m_Particle3.SizeVariation = 0.1f, m_Particle3.SizeEnd = 0.0f;
	m_Particle3.LifeTime = 1.0f;
	m_Particle3.Velocity = { 0.0f, 1.0f };
	m_Particle3.VelocityVariation = { 3.0f, 1.0f };
	m_Particle3.Position = { 0.0f, 0.0f };

	m_Particle4.Position = { 3.0, 0.0f };
	m_Particle4.ColorBegin = { 0.0f, 1.0f, 0.0f, 1.0f };
	m_Particle4.ColorEnd = { 1.0f, 1.0f, 1.0f, 0.0f };
	m_Particle4.SizeBegin = 0.2f, m_Particle4.SizeVariation = 0.1f, m_Particle4.SizeEnd = 0.0f;
	m_Particle4.LifeTime = 1.0f;
	m_Particle4.Velocity = { 1.0f, 0.0f };
	m_Particle4.VelocityVariation = { 3.0f, 1.0f };
	m_Particle4.Position = { 0.0f, 0.0f };

	m_Particle5.Position = { -3.0, 0.0f };
	m_Particle5.ColorBegin = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_Particle5.ColorEnd = { 1.0f, 0.0f, 0.0f, 0.0f };
	m_Particle5.SizeBegin = 0.2f, m_Particle5.SizeVariation = 0.1f, m_Particle5.SizeEnd = 0.0f;
	m_Particle5.LifeTime = 1.0f;
	m_Particle5.Velocity = { -1.0f, 0.0f };
	m_Particle5.VelocityVariation = { 3.0f, 1.0f };
	m_Particle5.Position = { 0.0f, 0.0f };

}

void TestLayer::OnDetach()
{
	VS_PROFILE_FUNCTION();
}

void TestLayer::OnUpdate(Venus::Timestep ts)
{
	VS_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(ts);

	// Particles
	if (Venus::Input::IsMouseButtonPressed(Venus::Mouse::ButtonLeft))
	{
		auto x = Venus::Input::GetMousePosition().x;
		auto y = Venus::Input::GetMousePosition().y;

		auto width = Venus::Application::Get().GetWindow().GetWidth();
		auto height = Venus::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameraController.GetBounds();
		auto pos = m_CameraController.GetCamera().GetPosition();

		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();

		m_Particle.Position = { x + pos.x, y + pos.y };

		m_ParticleSystem.Emit(m_Particle, m_EmitQuantity);
	}

	m_ParticleSystem.Emit(m_Particle2, 10);
	m_ParticleSystem.Emit(m_Particle3, 10);
	m_ParticleSystem.Emit(m_Particle4, 10);
	m_ParticleSystem.Emit(m_Particle5, 10);
	
	m_ParticleSystem.OnUpdate(ts);

	// Render
	Venus::Renderer2D::ResetStats();
	{
		VS_PROFILE_SCOPE("Renderer Prep");
		Venus::RenderCommand::SetClearColor(m_ClearColor);
		Venus::RenderCommand::Clear();
	}

	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;	
		
		VS_PROFILE_SCOPE("Renderer Draw");

		m_ParticleSystem.OnRender(m_CameraController.GetCamera());

		Venus::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Venus::Renderer2D::DrawRect({ 0.0f, 0.0f, 0.0f }, { 20.0f, 20.0f }, { 1.0f, 1.0f, 1.0f, 0.9f });
		Venus::Renderer2D::DrawLine({ 0.0f, -10.0f, 0.0f }, { 0.0f, 10.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 0.9f });
		Venus::Renderer2D::DrawCircle({ -0.0f, 0.0f }, m_CircleScale, { 0.3f, 0.3f, 0.3f, 1.0f }, m_CircleThickness);
		Venus::Renderer2D::DrawQuad(m_QuadTransform, { 1.0f, 1.0f }, m_CheckerboardTexture, 20.0f);
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.5f };
				Venus::Renderer2D::DrawRotatedQuad({ x, y }, { 0.45f, 0.45f }, rotation, color);
			}
		}
		Venus::Renderer2D::EndScene();
	}
}

void TestLayer::OnImGuiRender()
{
	ImGui::Begin("Stats");
	auto stats = Venus::Renderer2D::GetStats();
	ImGui::Text(Venus::RendererAPI::GetAPIName().c_str());
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::End();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_ClearColor));
	ImGui::DragFloat3("Transform", glm::value_ptr(m_QuadTransform), 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::DragFloat2("Circle Scale", glm::value_ptr(m_CircleScale), 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::DragFloat("Circle Thickness", &m_CircleThickness, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::Separator();
	ImGui::DragInt("Quantity", &m_EmitQuantity, 0.1f, 0, 10000);
	ImGui::ColorEdit4("Birth Color", glm::value_ptr(m_Particle.ColorBegin));
	ImGui::ColorEdit4("Death Color", glm::value_ptr(m_Particle.ColorEnd));
	ImGui::DragFloat("Life Time", &m_Particle.LifeTime, 0.1f, 0.0f, 1000.0f);
	ImGui::End();
}

void TestLayer::OnEvent(Venus::Event& e)
{
	m_CameraController.OnEvent(e);
}
