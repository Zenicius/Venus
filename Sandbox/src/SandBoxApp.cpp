#include "Venus.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

class TestLayer : public Venus::Layer
{
	public:
		TestLayer::TestLayer()
			:Layer("TestSandBox"), m_CameraController(1280.0f / 720.0f)
		{
		}

		void TestLayer::OnAttach() override
		{
			Venus::ResourceManager::LoadShader("assets/shaders/default2D.vert", "assets/shaders/default2D.frag", "default");
			Venus::ResourceManager::LoadShader("assets/shaders/defaultText.vert", "assets/shaders/defaultText.frag", "defaultText");
			Venus::ResourceManager::LoadTexture("assets/textures/pyramid.jpg", "pyramid");
			Venus::ResourceManager::LoadTexture("assets/textures/brick.jpg", "brick");
			Venus::ResourceManager::LoadFont("assets/fonts/Coco.ttf", 24);

			Venus::Renderer2D::Init(Venus::ResourceManager::GetShader("default"), 
									Venus::ResourceManager::GetShader("defaultText"));
		}

		void TestLayer::OnDetach() override
		{
			Venus::ResourceManager::Clear();
		}

		void TestLayer::OnUpdate(Venus::Timestep ts) override
		{
			m_CameraController.OnUpdate(ts);

			Venus::Renderer2D::SetClearColor(m_ClearColor);
			Venus::Renderer2D::Clear();
			Venus::Renderer2D::StartScene(m_CameraController.GetCamera());
			Venus::Renderer2D::DrawSprite(Venus::ResourceManager::GetTexture("pyramid"), m_Transform, {1.0f, 1.0f});
			Venus::Renderer2D::DrawSprite(Venus::ResourceManager::GetTexture("brick"), m_Transform2, { 1.0f, 1.0f });
			Venus::Renderer2D::DrawText("Oi", 5.0f, 10.0f, 1.0f, { 1.0f, 1.0f, 1.0f });
		}

		void TestLayer::OnImGuiRender() override
		{
			std::string cameraPos = "X: " + std::to_string(m_CameraController.GetCamera().GetPosition().x) 
								+ " Y: " + std::to_string(m_CameraController.GetCamera().GetPosition().y);

			ImGui::Begin("Debug");
			ImGui::LabelText("", "Camera");
			ImGui::Text(cameraPos.c_str());
			ImGui::LabelText("", "Scene");
			ImGui::ColorEdit3("Clear Color", glm::value_ptr(m_ClearColor));
			ImGui::LabelText("", "Sprite 1");
			ImGui::DragFloat("X", &m_Transform.x, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::DragFloat("Y", &m_Transform.y, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::LabelText("", "Sprite 2");
			ImGui::DragFloat("X 2", &m_Transform2.x, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::DragFloat("Y 2", &m_Transform2.y, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::End();
		}

	private:
		Venus::OrthographicCameraController m_CameraController;

		glm::vec2 m_Transform = { 0.0f, 0.0f };
		glm::vec2 m_Transform2 = { 0.0f, 0.0f };
		glm::vec3 m_ClearColor = { 0.1f, 0.1f, 0.1f };
};

class Sandbox : public Venus::Application
{
	public:
		Sandbox()
		{
			PushLayer(new TestLayer());
		}

		~Sandbox()
		{

		}
};

Venus::Application* Venus::CreateApplication()
{
	return new Sandbox();
}