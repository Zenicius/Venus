#include "DevLayer.h"

#include <glm/gtc/type_ptr.hpp>

DevLayer::DevLayer()
{
}

void DevLayer::OnAttach()
{
	//m_ActiveModel = Venus::Model("assets/models/backpack/backpack.obj");
	//m_ActiveModel = Venus::Model("assets/models/cubo.obj");
	//m_ActiveModel = Venus::Model("assets/models/sword/sword.fbx");
	//m_ActiveModel = Venus::Model("assets/models/grindstone/grindstone.fbx");
	//m_ActiveModel = Venus::Model("assets/models/wooden/Wooden.obj");
	//m_ActiveModel = Venus::Model("assets/models/fish/fish.obj");
	//m_ActiveModel = Venus::Model("assets/models/house/house.obj");
	//m_ActiveModel = Venus::Model("assets/models/cucumber/Cucumber.obj");
	//m_ActiveModel = Venus::CreateRef<Venus::Model>("assets/models/trator/trator.obj");
	//m_ActiveModel = Venus::Model("assets/models/onion/onion.obj");
	//m_ActiveModel = Venus::Factory::CreateCube(glm::vec3(1.0f));

	m_EditorCamera = Venus::EditorCamera(30.0f, 1600.f / 900.0f, 0.1f, 1000.0f);
}

void DevLayer::OnDetach()
{
}

void DevLayer::OnUpdate(Venus::Timestep ts)
{
}

void DevLayer::OnImGuiRender()
{
	ImGui::Begin("Model");
	RenderVec3Control("Position", m_ModelTransform.Position, 0.0f);

	glm::vec3 rotation = glm::degrees(m_ModelTransform.Rotation);
	RenderVec3Control("Rotation", rotation);
	m_ModelTransform.Rotation = glm::radians(rotation);

	RenderVec3Control("Scale", m_ModelTransform.Scale, 1.0f);

	ImGui::Checkbox("Rotate", &m_RotateModel);

	ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_ClearColor));

	ImGui::Separator();
	auto stats = Venus::Renderer::GetStats();
	ImGui::Text("Renderer");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Vertices: %d", stats.VertexCount);
	ImGui::Text("Indices: %d", stats.IndexCount);

	ImGui::Separator();
	auto stats2D = Venus::Renderer2D::GetStats();
	ImGui::Text("Renderer2D");
	ImGui::Text("Draw Calls: %d", stats2D.DrawCalls);
	ImGui::Text("Quads: %d", stats2D.QuadCount);
	ImGui::Text("Vertices: %d", stats2D.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats2D.GetTotalIndexCount());

	ImGui::End();
}

void DevLayer::OnEvent(Venus::Event& e)
{
	m_EditorCamera.OnEvent(e);
}
